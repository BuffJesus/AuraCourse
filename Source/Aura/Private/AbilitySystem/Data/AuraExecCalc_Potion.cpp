// Not Sure Yet

#include "AbilitySystem/Data/AuraExecCalc_Potion.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

struct AuraPotionStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxMana);
	
	AuraPotionStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, MaxHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, MaxMana, Target, false);
	}
};

static const AuraPotionStatics& PotionStatics()
{
	static const AuraPotionStatics PotionStatics;
	return PotionStatics;
}

UAuraExecCalc_Potion::UAuraExecCalc_Potion()
{
	RelevantAttributesToCapture.Add(PotionStatics().MaxHealthDef);
	RelevantAttributesToCapture.Add(PotionStatics().MaxManaDef);
}

void UAuraExecCalc_Potion::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// Helper lambda to capture attributes
	auto CaptureAttribute = [&ExecutionParams](const FGameplayEffectAttributeCaptureDefinition& CaptureDef, 
		const FAggregatorEvaluateParameters& EvalParams) -> float
	{
		float Value { 0.f };
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvalParams, Value);
		return FMath::Max(Value, 0.f);
	};

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	// Capture max vital attributes for potential percentage-based calculations
	const float TargetMaxHealth = CaptureAttribute(PotionStatics().MaxHealthDef, EvalParams);
	const float TargetMaxMana = CaptureAttribute(PotionStatics().MaxManaDef, EvalParams);

	// Get healing amounts from SetByCaller
	const float HealthRestoration = Spec.GetSetByCallerMagnitude(Aura::Potion::HealthRestoration, false, 0.f);
	const float ManaRestoration = Spec.GetSetByCallerMagnitude(Aura::Potion::ManaRestoration, false, 0.f);

	// Apply health restoration if specified
	if (HealthRestoration > 0.f)
	{
		// Optionally: Add level scaling or stat-based bonuses here
		// For now, use the raw value from SetByCaller
		
		const FGameplayModifierEvaluatedData HealthData(
			UAuraAttributeSet::GetHealthAttribute(), 
			EGameplayModOp::Additive, 
			HealthRestoration
		);
		OutExecutionOutput.AddOutputModifier(HealthData);
		
		UE_LOG(LogTemp, Log, TEXT("Potion restored %.2f Health (Max: %.2f)"), HealthRestoration, TargetMaxHealth);
	}

	// Apply mana restoration if specified
	if (ManaRestoration > 0.f)
	{
		// Optionally: Add level scaling or intelligence-based bonuses here
		// For now, use the raw value from SetByCaller
		
		const FGameplayModifierEvaluatedData ManaData(
			UAuraAttributeSet::GetManaAttribute(), 
			EGameplayModOp::Additive, 
			ManaRestoration
		);
		OutExecutionOutput.AddOutputModifier(ManaData);
		
		UE_LOG(LogTemp, Log, TEXT("Potion restored %.2f Mana (Max: %.2f)"), ManaRestoration, TargetMaxMana);
	}
}