// Not Sure Yet


#include "AbilitySystem/ModMagCalc/AuraMMC_MaxMana.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/AuraCombatInterface.h"

UAuraMMC_MaxMana::UAuraMMC_MaxMana()
{
	IntDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute();
	IntDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IntDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(IntDef);
}

float UAuraMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags { Spec.CapturedSourceTags.GetAggregatedTags() };
	const FGameplayTagContainer* TargetTags { Spec.CapturedTargetTags.GetAggregatedTags() };

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	float Int { 0 };
	GetCapturedAttributeMagnitude(IntDef, Spec, EvalParams, Int);
	Int = FMath::Max<float>(Int, 0.f);

	IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel { CombatInterface->GetPlayerLevel() };

	return 100.f + 8.f * Int + 15.f * PlayerLevel;
}
