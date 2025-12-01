// Not Sure Yet

#include "AbilitySystem/ExecCalc/AuraExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "AbilitySystem/Attributes/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraCharacterClassInfo.h"
#include "AbilitySystem/Data/AuraDamageCalcConfig.h"
#include "GameplayTagsManager.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"
#include "Async/ParallelFor.h"
#include "Misc/ScopeLock.h"

// Default config values as fallback
namespace DamageCalcDefaults
{
    constexpr float BlockDamageReduction = 0.5f;
    constexpr float CriticalHitMultiplier = 2.0f;
    constexpr float LuckToArmorPenetrationRatio = 1.0f;
    constexpr float LuckToCriticalHitChanceRatio = 1.0f;
    // Crit safety clamp defaults
    constexpr float MinCriticalHitMultiplier = 1.0f;   // at least normal damage on crit
    constexpr float MaxCriticalHitMultiplier = 3.0f;   // cap extreme stacking
    // Debug logging default (used if no config asset provided)
    constexpr bool bEnableDamageDebugLogs = true;
    const FName ArmorPenCurveName("ArmorPenetration");
    const FName EffectiveArmorCurveName("EffectiveArmor");
    const FName CriticalHitResistanceCurveName("CriticalHitResistance");
}

// PARALLEL DAMAGE CALCULATION STRUCTURES
struct FDamageTypeResult
{
	FGameplayTag DamageType;
	float RawDamage;
	float Resistance;
	float MitigatedDamage;
	
	FDamageTypeResult()
		: RawDamage(0.f)
		, Resistance(0.f)
		, MitigatedDamage(0.f)
	{}
};

// Thread-safe accumulator for parallel damage calculation
class FParallelDamageAccumulator
{
public:
	FParallelDamageAccumulator() : TotalDamage(0.f) {}
	
	void AddDamage(float Damage)
	{
		FScopeLock Lock(&CriticalSection);
		TotalDamage += Damage;
	}
	
	float GetTotalDamage() const
	{
		FScopeLock Lock(&CriticalSection);
		return TotalDamage;
	}
	
private:
	mutable FCriticalSection CriticalSection;
	float TotalDamage;
};

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Luck);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Luck, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static const AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UAuraExecCalc_Damage::UAuraExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LuckDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UAuraExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Use config if available, otherwise fallback to defaults
	const UAuraDamageCalcConfig* Config { GetDamageConfig() };
	const bool bHasConfig = (Config != nullptr);
	
	if (!bHasConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageConfig is NULL! Using default values for damage calculation."));
	}
	
	// Setup and Helper Lambdas
	auto GetAvatarFromASC = [](const UAbilitySystemComponent* ASC) -> const AActor*
	{
		return ASC ? ASC->GetAvatarActor() : nullptr;
	};

	// Helper lambda to capture and clamp attributes
	auto CaptureAttribute = [&ExecutionParams](const FGameplayEffectAttributeCaptureDefinition& CaptureDef, 
		const FAggregatorEvaluateParameters& EvalParams) -> float
	{
		float Value { 0.f };
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvalParams, Value);
		return FMath::Max(Value, 0.f);
	};

	// Helper lambda to safely get curve coefficients
	auto GetCurveCoefficient = [](const UCurveTable* CurveTable, const FName& CurveName, 
		const int32 Level, const float DefaultValue = 1.f) -> float
	{
		if (!CurveTable)
		{
			return DefaultValue;
		}
		
		const FRealCurve* Curve { CurveTable->FindCurve(CurveName, FString()) };
		if (!Curve)
		{
			return DefaultValue;
		}
		
		return Curve->Eval(Level);
	};

	const AActor* SourceAvatar { GetAvatarFromASC(ExecutionParams.GetSourceAbilitySystemComponent()) };
	const AActor* TargetAvatar { GetAvatarFromASC(ExecutionParams.GetTargetAbilitySystemComponent()) };
	const IAuraCombatInterface* SourceCombatInterface { Cast<IAuraCombatInterface>(SourceAvatar) };
	const IAuraCombatInterface* TargetCombatInterface { Cast<IAuraCombatInterface>(TargetAvatar) };
	
	if (!SourceCombatInterface || !TargetCombatInterface) { return; }
	
	const FGameplayEffectSpec Spec { ExecutionParams.GetOwningSpec() };
	const FGameplayTagContainer* SourceTags { Spec.CapturedSourceTags.GetAggregatedTags() };
	const FGameplayTagContainer* TargetTags { Spec.CapturedTargetTags.GetAggregatedTags() };
	
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;
	
	const UAuraCharacterClassInfo* CharacterClassInfo { UAuraAbilitySystemBPLibrary::GetCharacterClassInfo(SourceAvatar) };
	if (!CharacterClassInfo || !CharacterClassInfo->DamageCalcCoefficients)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterClassInfo or DamageCalcCoefficients is NULL! Cannot execute damage calculation."));
		return;
	}
	
	const int32 SourceLevel { SourceCombatInterface->GetCharacterLevel() };
	const int32 TargetLevel { TargetCombatInterface->GetCharacterLevel() };
	
	// PARALLEL DAMAGE TYPE PROCESSING
	float Damage { 0.f };
	const FGameplayTagContainer AllDamageTypes { UGameplayTagsManager::Get().RequestGameplayTagChildren(Aura::Damage::Damage) };
	
	// Convert to array for parallel processing
	TArray<FGameplayTag> DamageTypeArray;
	AllDamageTypes.GetGameplayTagArray(DamageTypeArray);
	
	const int32 NumDamageTypes = DamageTypeArray.Num();
	
	// Only use parallel if beneficial (3+ damage types)
	const bool bUseParallel = (NumDamageTypes >= 3);
	
	// Pre-allocate results array
	TArray<FDamageTypeResult> DamageResults;
	DamageResults.SetNum(NumDamageTypes);
	
	if (bUseParallel)
	{
		// PARALLEL VERSION: Process damage types on multiple threads
		TRACE_CPUPROFILER_EVENT_SCOPE(ParallelDamageCalculation);
		
		FParallelDamageAccumulator DamageAccumulator;
		
		ParallelFor(NumDamageTypes, [&](int32 Index)
		{
			const FGameplayTag& DamageType = DamageTypeArray[Index];
			FDamageTypeResult& Result = DamageResults[Index];
			Result.DamageType = DamageType;
			
			Result.RawDamage = Spec.GetSetByCallerMagnitude(DamageType, false, 0.f);
			
			if (Result.RawDamage <= 0.f)
			{
				return;
			}
			
			// Get resistance value
			float Resistance { 0.f };
			
			#define CAPTURE_RESISTANCE(Type) \
				if (DamageType.MatchesTagExact(Aura::Damage::Type)) \
				{ \
					Resistance = CaptureAttribute(DamageStatics().Type##ResistanceDef, EvalParams); \
				}
			
			CAPTURE_RESISTANCE(Fire)
			CAPTURE_RESISTANCE(Lightning)
			CAPTURE_RESISTANCE(Arcane)
			CAPTURE_RESISTANCE(Physical)
			
			#undef CAPTURE_RESISTANCE
			
			Result.Resistance = Resistance;
			
			// Apply resistance (clamped to 75% max reduction)
			const float ResistanceMultiplier { 1.f - FMath::Clamp(Resistance / 100.f, 0.f, 0.75f) };
			Result.MitigatedDamage = Result.RawDamage * ResistanceMultiplier;
			
			// Thread-safe accumulation
			DamageAccumulator.AddDamage(Result.MitigatedDamage);
		});
		
		Damage = DamageAccumulator.GetTotalDamage();
	}
	else
	{
		// SEQUENTIAL VERSION: Use for small number of damage types
		TRACE_CPUPROFILER_EVENT_SCOPE(SequentialDamageCalculation);
		
		for (int32 Index = 0; Index < NumDamageTypes; ++Index)
		{
			const FGameplayTag& DamageType = DamageTypeArray[Index];
			FDamageTypeResult& Result = DamageResults[Index];
			Result.DamageType = DamageType;
			
			Result.RawDamage = Spec.GetSetByCallerMagnitude(DamageType, false, 0.f);
			if (Result.RawDamage <= 0.f) { continue; }
			
			float Resistance { 0.f };
			
			#define CAPTURE_RESISTANCE(Type) \
				if (DamageType.MatchesTagExact(Aura::Damage::Type)) \
				{ \
					Resistance = CaptureAttribute(DamageStatics().Type##ResistanceDef, EvalParams); \
				}
			
			CAPTURE_RESISTANCE(Fire)
			CAPTURE_RESISTANCE(Lightning)
			CAPTURE_RESISTANCE(Arcane)
			CAPTURE_RESISTANCE(Physical)
			
			#undef CAPTURE_RESISTANCE
			
			Result.Resistance = Resistance;
			
			const float ResistanceMultiplier { 1.f - FMath::Clamp(Resistance / 100.f, 0.f, 0.75f) };
			Result.MitigatedDamage = Result.RawDamage * ResistanceMultiplier;
			Damage += Result.MitigatedDamage;
		}
	}
	
	// Log damage type results
	for (const FDamageTypeResult& Result : DamageResults)
	{
		if (Result.RawDamage > 0.f)
		{
			UE_LOG(LogTemp, Verbose, TEXT("DamageType: %s | Raw: %.2f | Resistance: %.2f%% | Mitigated: %.2f"), 
				*Result.DamageType.ToString(), Result.RawDamage, Result.Resistance, Result.MitigatedDamage);
		}
	}
	
	// Early out if no damage
	if (Damage <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("No damage set via SetByCaller! Aborting damage calculation."));
		return;
	}
	
	// Store initial damage for logging
	const float InitialDamage { Damage };
	
	const float SourceLuck { CaptureAttribute(DamageStatics().LuckDef, EvalParams) };
	const float TargetBlockChance { CaptureAttribute(DamageStatics().BlockChanceDef, EvalParams) };
	
	// Block Chance
	const bool bBlocked { FMath::FRandRange(UE_SMALL_NUMBER, 100.f) <= TargetBlockChance };
	
	FGameplayEffectContextHandle EffectContextHandle { Spec.GetContext() };
	UAuraAbilitySystemBPLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	const float BlockReduction = bHasConfig ? Config->BlockDamageReduction : DamageCalcDefaults::BlockDamageReduction;
	Damage = bBlocked ? Damage * BlockReduction : Damage;
	
	// Armor Mitigation
	const float TargetArmor { CaptureAttribute(DamageStatics().ArmorDef, EvalParams) };
	float SourceArmorPenetration { CaptureAttribute(DamageStatics().ArmorPenetrationDef, EvalParams) };
	
	const bool bLuckAffectsArmorPen = bHasConfig ? Config->bLuckAffectsArmorPenetration : true;
	const float LuckToArmorPenRatio = bHasConfig ? Config->LuckToArmorPenetrationRatio : DamageCalcDefaults::LuckToArmorPenetrationRatio;
	
	if (bLuckAffectsArmorPen)
	{
		SourceArmorPenetration += SourceLuck * LuckToArmorPenRatio;
	}
	
	const FName ArmorPenCurveName = bHasConfig ? Config->ArmorPenCurveName : DamageCalcDefaults::ArmorPenCurveName;
	const FName EffectiveArmorCurveName = bHasConfig ? Config->EffectiveArmorCurveName : DamageCalcDefaults::EffectiveArmorCurveName;
	
	const float ArmorPenCoefficient { GetCurveCoefficient(CharacterClassInfo->DamageCalcCoefficients, 
		ArmorPenCurveName, SourceLevel) };
	const float EffectiveArmor { TargetArmor * (100.f - SourceArmorPenetration * ArmorPenCoefficient) / 100.f };
	const float EffectiveArmorCoefficient { GetCurveCoefficient(CharacterClassInfo->DamageCalcCoefficients, 
		EffectiveArmorCurveName, TargetLevel) };
	
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	// Critical Hit
	float SourceCriticalHitChance { CaptureAttribute(DamageStatics().CriticalHitChanceDef, EvalParams) };
	
	const bool bLuckAffectsCritChance = bHasConfig ? Config->bLuckAffectsCriticalHitChance : true;
	const float LuckToCritChanceRatio = bHasConfig ? Config->LuckToCriticalHitChanceRatio : DamageCalcDefaults::LuckToCriticalHitChanceRatio;
	
	if (bLuckAffectsCritChance)
	{
		SourceCriticalHitChance += SourceLuck * LuckToCritChanceRatio;
	}
	
	const float TargetCriticalHitResistance { CaptureAttribute(DamageStatics().CriticalHitResistanceDef, EvalParams) };
	const float EffectiveCriticalHitResistance { TargetCriticalHitResistance * (100.f - SourceArmorPenetration * ArmorPenCoefficient) / 100.f };
	const float CriticalHitResistanceCoefficient { GetCurveCoefficient(CharacterClassInfo->DamageCalcCoefficients, 
		DamageCalcDefaults::CriticalHitResistanceCurveName, TargetLevel) };
	
	float EffectiveCriticalHitChance { SourceCriticalHitChance * (100.f - EffectiveCriticalHitResistance * CriticalHitResistanceCoefficient) / 100.f };
	EffectiveCriticalHitChance = FMath::Max(0.f, EffectiveCriticalHitChance);
	
	const bool bCriticalHit { FMath::FRandRange(UE_SMALL_NUMBER, 100.f) <= EffectiveCriticalHitChance };
 if (bCriticalHit)
 {
     UAuraAbilitySystemBPLibrary::SetIsCriticalHit(EffectContextHandle, true);
        
     // Interpret CriticalHitDamage as a bonus to the crit multiplier (option 2)
     // Backward-compatible: if the attribute is configured as a percentage (0-100), normalize to 0-1.
     const float SourceCriticalHitDamage { CaptureAttribute(DamageStatics().CriticalHitDamageDef, EvalParams) };
     const float BaseCritMultiplier = bHasConfig ? Config->CriticalHitMultiplier : DamageCalcDefaults::CriticalHitMultiplier;
     float CritBonus = SourceCriticalHitDamage;
     if (CritBonus > 1.f)
     {
         CritBonus *= 0.01f; // convert 0-100% into 0-1 range
     }

     // Final hybrid multiplier = BaseCritMultiplier + CritBonus (e.g., 2.0 + 0.5 = 2.5x)
     const float FinalCritMultiplier = BaseCritMultiplier + CritBonus;

     // Clamp to safety range from config or defaults
     const float MinCrit = bHasConfig ? Config->MinCriticalHitMultiplier : DamageCalcDefaults::MinCriticalHitMultiplier;
     const float MaxCrit = bHasConfig ? Config->MaxCriticalHitMultiplier : DamageCalcDefaults::MaxCriticalHitMultiplier;
     const float ClampedCritMultiplier = FMath::Clamp(FinalCritMultiplier, MinCrit, MaxCrit);
     const bool bWasClamped = !FMath::IsNearlyEqual(ClampedCritMultiplier, FinalCritMultiplier);

     Damage *= ClampedCritMultiplier;

     // Logging for crits
     const bool bDebugLogs = bHasConfig ? Config->bEnableDamageDebugLogs : DamageCalcDefaults::bEnableDamageDebugLogs;
     if (bDebugLogs)
     {
         if (bWasClamped)
         {
             UE_LOG(LogTemp, Warning, TEXT("[CRIT] Multiplier clamped from %.2f to %.2f (Range: %.2f–%.2f)"), FinalCritMultiplier, ClampedCritMultiplier, MinCrit, MaxCrit);
         }
         UE_LOG(LogTemp, Warning, TEXT("*** CRITICAL HIT! *** Damage: %.2f (Crit Mult: %.2f | Bonus: %.2f%%)"), Damage, ClampedCritMultiplier, CritBonus * 100.f);
     }
  }
    
     // Debug Logging
     {
         const bool bDebugLogs = bHasConfig ? Config->bEnableDamageDebugLogs : DamageCalcDefaults::bEnableDamageDebugLogs;
         if (bDebugLogs)
         {
             UE_LOG(LogTemp, Warning, TEXT("=== DAMAGE CALC DEBUG ==="));
             UE_LOG(LogTemp, Warning, TEXT("Config: %s | Parallel: %s"), 
                 bHasConfig ? TEXT("Valid") : TEXT("Defaults"),
                 bUseParallel ? TEXT("Yes") : TEXT("No"));
             UE_LOG(LogTemp, Warning, TEXT("Source Level: %d | Target Level: %d | Luck: %.2f"), SourceLevel, TargetLevel, SourceLuck);
             UE_LOG(LogTemp, Warning, TEXT("Initial Damage: %.2f | Blocked: %s"), InitialDamage, bBlocked ? TEXT("Yes") : TEXT("No"));
             UE_LOG(LogTemp, Warning, TEXT("Armor: %.2f | ArmorPen: %.2f | Effective: %.2f"), TargetArmor, SourceArmorPenetration, EffectiveArmor);
             UE_LOG(LogTemp, Warning, TEXT("CritChance: %.2f%% | CritResist: %.2f | Effective: %.2f%%"), SourceCriticalHitChance, TargetCriticalHitResistance, EffectiveCriticalHitChance);
             UE_LOG(LogTemp, Warning, TEXT("Final Damage: %.2f"), Damage);
             UE_LOG(LogTemp, Warning, TEXT("========================="));
         }
     }
	
	// Apply Damage
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}