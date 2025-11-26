// Not Sure Yet


#include "AbilitySystem/ExecCalc/AuraExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraCharacterClassInfo.h"
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
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
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
}

void UAuraExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	auto GetAvatarFromASC = [](const UAbilitySystemComponent* ASC) -> const AActor*
    {
    	return ASC ? ASC->GetAvatarActor() : nullptr;
    };

    const AActor* SourceAvatar { GetAvatarFromASC(ExecutionParams.GetSourceAbilitySystemComponent()) };
    const AActor* TargetAvatar { GetAvatarFromASC(ExecutionParams.GetTargetAbilitySystemComponent()) };
	const IAuraCombatInterface* SourceCombatInterface = Cast<IAuraCombatInterface>(SourceAvatar);
	const IAuraCombatInterface* TargetCombatInterface = Cast<IAuraCombatInterface>(TargetAvatar);
	
	// Null check to ensure we have valid combat interfaces
	if (!SourceCombatInterface || !TargetCombatInterface)
	{
		return;
	}
	
	const FGameplayEffectSpec Spec { ExecutionParams.GetOwningSpec() };
	
	const FGameplayTagContainer* SourceTags { Spec.CapturedSourceTags.GetAggregatedTags() };
	const FGameplayTagContainer* TargetTags { Spec.CapturedTargetTags.GetAggregatedTags() };
	
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;
	
	// Get Damage Set by Caller Magnitude
	float Damage { Spec.GetSetByCallerMagnitude(Aura::Damage::Damage) };
	
	// Capture TargetBlockChance on Target, determine if successful
	float TargetBlockChance { 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvalParams, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);
	
	const bool bBlocked { FMath::FRandRange(UE_SMALL_NUMBER, 100.f) <= TargetBlockChance };
	Damage = bBlocked ? Damage * 0.5f : Damage;
	
	float TargetArmor { 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParams, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);
	
	float SourceArmorPenetration { 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvalParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	const UAuraCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemBPLibrary::GetCharacterClassInfo(SourceAvatar);
	if (!CharacterClassInfo) 
	{ 
		UE_LOG(LogTemp, Error, TEXT("CharacterClassInfo is NULL!"));
		return; 
	}

	if (!CharacterClassInfo->DamageCalcCoefficients)
	{
		UE_LOG(LogTemp, Error, TEXT("DamageCalcCoefficients is NULL!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("DamageCalcCoefficients: %s"), *CharacterClassInfo->DamageCalcCoefficients->GetName());

	const FRealCurve* ArmorPenCurve { CharacterClassInfo->DamageCalcCoefficients->FindCurve(FName("ArmorPenetration"), FString()) };
	if (!ArmorPenCurve) 
	{ 
		UE_LOG(LogTemp, Error, TEXT("ArmorPenCurve not found!"));
		return; 
	}
	
	const int32 SourceLevel = SourceCombatInterface->GetCharacterLevel();
	const int32 TargetLevel = TargetCombatInterface->GetCharacterLevel();
	const float ArmorPenCoefficient { ArmorPenCurve->Eval(SourceLevel) };
	
	UE_LOG(LogTemp, Warning, TEXT("=== DAMAGE CALC DEBUG ==="));
	UE_LOG(LogTemp, Warning, TEXT("Initial Damage: %.2f"), Damage);
	UE_LOG(LogTemp, Warning, TEXT("Source Level: %d, Target Level: %d"), SourceLevel, TargetLevel);
	UE_LOG(LogTemp, Warning, TEXT("TargetArmor: %.2f, SourceArmorPen: %.2f"), TargetArmor, SourceArmorPenetration);
	UE_LOG(LogTemp, Warning, TEXT("ArmorPenCoefficient (at level %d): %.4f"), SourceLevel, ArmorPenCoefficient);
	
	const float EffectiveArmor { TargetArmor * (100 - SourceArmorPenetration * ArmorPenCoefficient) / 100.f };
	UE_LOG(LogTemp, Warning, TEXT("EffectiveArmor: %.2f"), EffectiveArmor);

	const FRealCurve* EffectiveArmorCurve { CharacterClassInfo->DamageCalcCoefficients->FindCurve(FName("EffectiveArmor"), FString()) };
	if (!EffectiveArmorCurve) 
	{ 
		UE_LOG(LogTemp, Error, TEXT("EffectiveArmorCurve not found!"));
		return; 
	}

	// Check if we're finding the same curve twice (this would be the bug!)
	UE_LOG(LogTemp, Warning, TEXT("Are curves the same? %s"), (ArmorPenCurve == EffectiveArmorCurve) ? TEXT("YES - THIS IS THE BUG!") : TEXT("No"));

	const float EffectiveArmorCoefficient { EffectiveArmorCurve->Eval(TargetLevel) };
	
	UE_LOG(LogTemp, Warning, TEXT("EffectiveArmorCoefficient (at level %d): %.4f"), TargetLevel, EffectiveArmorCoefficient);
	
	// Manual check to see what the curve should actually return
	UE_LOG(LogTemp, Warning, TEXT("Manual check - EffectiveArmorCurve at level 1: %.4f"), EffectiveArmorCurve->Eval(1));
	UE_LOG(LogTemp, Warning, TEXT("Manual check - EffectiveArmorCurve at level 10: %.4f"), EffectiveArmorCurve->Eval(10));
	UE_LOG(LogTemp, Warning, TEXT("Manual check - EffectiveArmorCurve at level 11: %.4f"), EffectiveArmorCurve->Eval(11));
	
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	UE_LOG(LogTemp, Warning, TEXT("Damage after armor: %.2f"), Damage);
	
	// === CRITICAL HIT CALCULATION ===
	
	// 1. Capture CriticalHitChance from Source
	float SourceCriticalHitChance { 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvalParams, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);
	
	// 2. Capture CriticalHitResistance from Target
	float TargetCriticalHitResistance { 0.f };
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvalParams, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);
	
	// 4. BONUS: Use CriticalHitResistanceCoefficient curve to scale resistance by target level
	const FRealCurve* CriticalHitResistanceCurve { CharacterClassInfo->DamageCalcCoefficients->FindCurve(FName("CriticalHitResistance"), FString()) };
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve ? CriticalHitResistanceCurve->Eval(TargetLevel) : 1.f;
	
	// Apply resistance to reduce crit chance (clamped to 0 minimum)
	const float EffectiveCriticalHitChance { FMath::Max(0.f, SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient) };
	
	UE_LOG(LogTemp, Warning, TEXT("CritChance: %.2f, CritResist: %.2f, CritResistCoef: %.2f, EffectiveCritChance: %.2f"), 
		SourceCriticalHitChance, TargetCriticalHitResistance, CriticalHitResistanceCoefficient, EffectiveCriticalHitChance);
	
	// Determine if this is a critical hit
	const bool bCriticalHit { FMath::FRandRange(UE_SMALL_NUMBER, 100.f) <= EffectiveCriticalHitChance };
	
	// 3. If critical hit, double damage and add CriticalHitDamage bonus
	if (bCriticalHit)
	{
		float SourceCriticalHitDamage { 0.f };
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvalParams, SourceCriticalHitDamage);
		SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);
		
		// Double damage plus add the CriticalHitDamage bonus
		Damage = Damage * 2.f + SourceCriticalHitDamage;
		
		UE_LOG(LogTemp, Warning, TEXT("*** CRITICAL HIT! *** Damage: %.2f (CritDamage Bonus: %.2f)"), Damage, SourceCriticalHitDamage);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Final Damage: %.2f"), Damage);
	UE_LOG(LogTemp, Warning, TEXT("========================="));
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
