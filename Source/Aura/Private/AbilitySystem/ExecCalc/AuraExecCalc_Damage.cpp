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
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
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
	if (!CharacterClassInfo) { return; }
	const FRealCurve* ArmorPenCurve { CharacterClassInfo->DamageCalcCoefficients->FindCurve(FName("ArmorPenetration"), FString()) };
	if (!ArmorPenCurve) { return; }
	const float ArmorPenCoefficient { ArmorPenCurve->Eval(SourceCombatInterface->GetCharacterLevel()) };
	
	const float EffectiveArmor { TargetArmor * (100 - SourceArmorPenetration * ArmorPenCoefficient) / 100.f };

	const FRealCurve* EffectiveArmorCurve { CharacterClassInfo->DamageCalcCoefficients->FindCurve(FName("EffectiveArmor"), FString()) };
	if (!EffectiveArmorCurve) { return; }
	const float EffectiveArmorCoefficient { EffectiveArmorCurve->Eval(TargetCombatInterface->GetCharacterLevel()) };
	
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
