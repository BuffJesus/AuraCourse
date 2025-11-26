// Not Sure Yet


#include "AbilitySystem/ExecCalc/AuraExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraCharacterClassInfo.h"
#include "AbilitySystem/Data/AuraDamageCalcConfig.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Luck);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Luck, Source, false);
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
}

void UAuraExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Validate DamageConfig
	const UAuraDamageCalcConfig* Config = GetDamageConfig();
	if (!Config)
	{
		UE_LOG(LogTemp, Error, TEXT("DamageConfig is NULL! Cannot execute damage calculation."));
		return;
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
			UE_LOG(LogTemp, Error, TEXT("CurveTable is NULL for curve: %s"), *CurveName.ToString());
			return DefaultValue;
		}
		
		const FRealCurve* Curve = CurveTable->FindCurve(CurveName, FString());
		if (!Curve)
		{
			UE_LOG(LogTemp, Error, TEXT("Curve not found: %s"), *CurveName.ToString());
			return DefaultValue;
		}
		
		return Curve->Eval(Level);
	};

	const AActor* SourceAvatar { GetAvatarFromASC(ExecutionParams.GetSourceAbilitySystemComponent()) };
	const AActor* TargetAvatar { GetAvatarFromASC(ExecutionParams.GetTargetAbilitySystemComponent()) };
	const IAuraCombatInterface* SourceCombatInterface = Cast<IAuraCombatInterface>(SourceAvatar);
	const IAuraCombatInterface* TargetCombatInterface = Cast<IAuraCombatInterface>(TargetAvatar);
	
	if (!SourceCombatInterface || !TargetCombatInterface) { return; }
	
	const FGameplayEffectSpec Spec { ExecutionParams.GetOwningSpec() };
	const FGameplayTagContainer* SourceTags { Spec.CapturedSourceTags.GetAggregatedTags() };
	const FGameplayTagContainer* TargetTags { Spec.CapturedTargetTags.GetAggregatedTags() };
	
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;
	
	const UAuraCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemBPLibrary::GetCharacterClassInfo(SourceAvatar);
	if (!CharacterClassInfo || !CharacterClassInfo->DamageCalcCoefficients)
	{
		UE_LOG(LogTemp, Error, TEXT("CharacterClassInfo or DamageCalcCoefficients is NULL!"));
		return;
	}
	
	const int32 SourceLevel = SourceCombatInterface->GetCharacterLevel();
	const int32 TargetLevel = TargetCombatInterface->GetCharacterLevel();
	
	// Attribute Capture
	float Damage { Spec.GetSetByCallerMagnitude(Aura::Damage::Damage) };
	const float SourceLuck { CaptureAttribute(DamageStatics().LuckDef, EvalParams) };
	const float TargetBlockChance { CaptureAttribute(DamageStatics().BlockChanceDef, EvalParams) };
	
	// Block Chance - Using Config
	const bool bBlocked { FMath::FRandRange(UE_SMALL_NUMBER, 100.f) <= TargetBlockChance };
	Damage = bBlocked ? Damage * Config->BlockDamageReduction : Damage;
	
	// Armor Mitigation - Using Config
	const float TargetArmor { CaptureAttribute(DamageStatics().ArmorDef, EvalParams) };
	float SourceArmorPenetration { CaptureAttribute(DamageStatics().ArmorPenetrationDef, EvalParams) };
	
	// Apply Luck to Armor Penetration - Using Config
	if (Config->bLuckAffectsArmorPenetration)
	{
		SourceArmorPenetration += SourceLuck * Config->LuckToArmorPenetrationRatio;
	}
	
	const float ArmorPenCoefficient { GetCurveCoefficient(CharacterClassInfo->DamageCalcCoefficients, 
		Config->ArmorPenCurveName, SourceLevel) };
	const float EffectiveArmor { TargetArmor * (100.f - SourceArmorPenetration * ArmorPenCoefficient) / 100.f };
	const float EffectiveArmorCoefficient { GetCurveCoefficient(CharacterClassInfo->DamageCalcCoefficients, 
		Config->EffectiveArmorCurveName, TargetLevel) };
	
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	// Critical Hit - Using Config
	float SourceCriticalHitChance { CaptureAttribute(DamageStatics().CriticalHitChanceDef, EvalParams) };
	
	// Apply Luck to Critical Hit Chance - Using Config
	if (Config->bLuckAffectsCriticalHitChance)
	{
		SourceCriticalHitChance += SourceLuck * Config->LuckToCriticalHitChanceRatio;
	}
	
	const float TargetCriticalHitResistance { CaptureAttribute(DamageStatics().CriticalHitResistanceDef, EvalParams) };
	const float EffectiveCriticalHitResistance { TargetCriticalHitResistance * (100.f - SourceArmorPenetration * ArmorPenCoefficient) / 100.f };
	const float CriticalHitResistanceCoefficient { GetCurveCoefficient(CharacterClassInfo->DamageCalcCoefficients, 
		FName("CriticalHitResistance"), TargetLevel) };
	
	float EffectiveCriticalHitChance { SourceCriticalHitChance * (100.f - EffectiveCriticalHitResistance * CriticalHitResistanceCoefficient) / 100.f };
	EffectiveCriticalHitChance = FMath::Max(0.f, EffectiveCriticalHitChance);
	
	const bool bCriticalHit { FMath::FRandRange(UE_SMALL_NUMBER, 100.f) <= EffectiveCriticalHitChance };
	if (bCriticalHit)
	{
		const float SourceCriticalHitDamage { CaptureAttribute(DamageStatics().CriticalHitDamageDef, EvalParams) };
		// Using Config for Crit Multiplier
		Damage = Damage * Config->CriticalHitMultiplier + SourceCriticalHitDamage;
		
		UE_LOG(LogTemp, Warning, TEXT("*** CRITICAL HIT! *** Damage: %.2f (CritDamage Bonus: %.2f)"), Damage, SourceCriticalHitDamage);
	}
	
	// Debug Logging
	UE_LOG(LogTemp, Warning, TEXT("=== DAMAGE CALC DEBUG ==="));
	UE_LOG(LogTemp, Warning, TEXT("Source Level: %d | Target Level: %d | Luck: %.2f"), SourceLevel, TargetLevel, SourceLuck);
	UE_LOG(LogTemp, Warning, TEXT("Initial Damage: %.2f | Blocked: %s"), Spec.GetSetByCallerMagnitude(Aura::Damage::Damage), bBlocked ? TEXT("Yes") : TEXT("No"));
	UE_LOG(LogTemp, Warning, TEXT("Armor: %.2f | ArmorPen: %.2f (+ Luck) | Effective: %.2f"), TargetArmor, SourceArmorPenetration, EffectiveArmor);
	UE_LOG(LogTemp, Warning, TEXT("CritChance: %.2f%% (+ Luck) | CritResist: %.2f | Effective: %.2f%%"), SourceCriticalHitChance, TargetCriticalHitResistance, EffectiveCriticalHitChance);
	UE_LOG(LogTemp, Warning, TEXT("Final Damage: %.2f"), Damage);
	UE_LOG(LogTemp, Warning, TEXT("========================="));
	
	// Apply Damage
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}