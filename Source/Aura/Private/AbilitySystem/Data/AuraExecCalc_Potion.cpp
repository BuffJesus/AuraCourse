// Not Sure Yet

#include "AbilitySystem/Data/AuraExecCalc_Potion.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AuraAttributeSet.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

struct AuraPotionStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxMana);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Vigor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Intelligence);
	
	AuraPotionStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, MaxHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, MaxMana, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Vigor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Intelligence, Target, false);
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
	RelevantAttributesToCapture.Add(PotionStatics().VigorDef);
	RelevantAttributesToCapture.Add(PotionStatics().IntelligenceDef);
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

	// Get character level for scaling
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(TargetAvatar);
	const int32 TargetLevel = CombatInterface ? CombatInterface->GetCharacterLevel() : 1;

	// Capture attributes
	const float TargetMaxHealth = CaptureAttribute(PotionStatics().MaxHealthDef, EvalParams);
	const float TargetMaxMana = CaptureAttribute(PotionStatics().MaxManaDef, EvalParams);
	const float TargetVigor = CaptureAttribute(PotionStatics().VigorDef, EvalParams);
	const float TargetIntelligence = CaptureAttribute(PotionStatics().IntelligenceDef, EvalParams);

	// Get base restoration amounts from SetByCaller
	float HealthRestoration = Spec.GetSetByCallerMagnitude(Aura::Potion::HealthRestoration, false, 0.f);
	float ManaRestoration = Spec.GetSetByCallerMagnitude(Aura::Potion::ManaRestoration, false, 0.f);

	// Apply health restoration with bonuses
	if (HealthRestoration > 0.f)
	{
		// Store base value for logging
		const float BaseHealthRestoration = HealthRestoration;
		
		// Apply level scaling
		if (bEnableLevelScaling && TargetLevel > 1)
		{
			const float LevelBonus = HealthRestoration * (TargetLevel - 1) * LevelScalingMultiplier;
			HealthRestoration += LevelBonus;
		}
		
		// Apply Vigor bonus
		if (bEnableVigorBonus && TargetVigor > 0.f)
		{
			const float VigorBonus = TargetVigor * VigorBonusMultiplier;
			HealthRestoration += VigorBonus;
		}
		
		// Apply the restoration
		const FGameplayModifierEvaluatedData HealthData(
			UAuraAttributeSet::GetHealthAttribute(), 
			EGameplayModOp::Additive, 
			HealthRestoration
		);
		OutExecutionOutput.AddOutputModifier(HealthData);
		
		// Debug logging
		UE_LOG(LogTemp, Log, TEXT("=== HEALTH POTION ==="));
		UE_LOG(LogTemp, Log, TEXT("Base: %.1f | Level %d Bonus: %.1f | Vigor (%.0f) Bonus: %.1f"), 
			BaseHealthRestoration,
			TargetLevel,
			bEnableLevelScaling ? (BaseHealthRestoration * (TargetLevel - 1) * LevelScalingMultiplier) : 0.f,
			TargetVigor,
			bEnableVigorBonus ? (TargetVigor * VigorBonusMultiplier) : 0.f
		);
		UE_LOG(LogTemp, Log, TEXT("Final Health Restored: %.1f (Max: %.1f)"), HealthRestoration, TargetMaxHealth);
	}

	// Apply mana restoration with bonuses
	if (ManaRestoration > 0.f)
	{
		// Store base value for logging
		const float BaseManaRestoration = ManaRestoration;
		
		// Apply level scaling
		if (bEnableLevelScaling && TargetLevel > 1)
		{
			const float LevelBonus = ManaRestoration * (TargetLevel - 1) * LevelScalingMultiplier;
			ManaRestoration += LevelBonus;
		}
		
		// Apply Intelligence bonus
		if (bEnableIntelligenceBonus && TargetIntelligence > 0.f)
		{
			const float IntelligenceBonus = TargetIntelligence * IntelligenceBonusMultiplier;
			ManaRestoration += IntelligenceBonus;
		}
		
		// Apply the restoration
		const FGameplayModifierEvaluatedData ManaData(
			UAuraAttributeSet::GetManaAttribute(), 
			EGameplayModOp::Additive, 
			ManaRestoration
		);
		OutExecutionOutput.AddOutputModifier(ManaData);
		
		// Debug logging
		UE_LOG(LogTemp, Log, TEXT("=== MANA POTION ==="));
		UE_LOG(LogTemp, Log, TEXT("Base: %.1f | Level %d Bonus: %.1f | Intelligence (%.0f) Bonus: %.1f"), 
			BaseManaRestoration,
			TargetLevel,
			bEnableLevelScaling ? (BaseManaRestoration * (TargetLevel - 1) * LevelScalingMultiplier) : 0.f,
			TargetIntelligence,
			bEnableIntelligenceBonus ? (TargetIntelligence * IntelligenceBonusMultiplier) : 0.f
		);
		UE_LOG(LogTemp, Log, TEXT("Final Mana Restored: %.1f (Max: %.1f)"), ManaRestoration, TargetMaxMana);
	}
}