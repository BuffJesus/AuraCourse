// Not Sure Yet

#include "AbilitySystem/ModMagCalc/AuraMMC_Base.h"
#include "Interaction/AuraCombatInterface.h"

UAuraMMC_Base::UAuraMMC_Base()
{
	// Don't call SetupCaptureDefinition here - AttributeToCapture isn't loaded yet
}

void UAuraMMC_Base::PostLoad()
{
	Super::PostLoad();
	
	// Setup capture definition after all properties are fully loaded from Blueprint
	SetupCaptureDefinition();
}

void UAuraMMC_Base::SetupCaptureDefinition()
{
	if (AttributeToCapture.IsValid())
	{
		AttributeDef.AttributeToCapture = AttributeToCapture;
		AttributeDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
		AttributeDef.bSnapshot = bSnapshot;

		RelevantAttributesToCapture.Empty();
		RelevantAttributesToCapture.Add(AttributeDef);
	}
}

float UAuraMMC_Base::ApplyOperation(float Value, float Coefficient, EMMCOperationType Operation) const
{
	switch (Operation)
	{
	case EMMCOperationType::Add:
		return Value + Coefficient;
		
	case EMMCOperationType::Multiply:
		return Value * Coefficient;
		
	case EMMCOperationType::Power:
		return FMath::Pow(Value, Coefficient);
		
	case EMMCOperationType::None:
	default:
		return 0.f;
	}
}

float UAuraMMC_Base::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags { Spec.CapturedSourceTags.GetAggregatedTags() };
	const FGameplayTagContainer* TargetTags { Spec.CapturedTargetTags.GetAggregatedTags() };

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	// Get captured attribute value
	float AttributeValue { 0.f };
	GetCapturedAttributeMagnitude(AttributeDef, Spec, EvalParams, AttributeValue);
	AttributeValue = FMath::Max<float>(AttributeValue, 0.f);

	// Get player level
	int32 PlayerLevel { 1 };
	if (IAuraCombatInterface* CombatInterface { Cast<IAuraCombatInterface>(Spec.GetContext().GetSourceObject()) })
	{
		PlayerLevel = CombatInterface->GetPlayerLevel();
	}

	// Calculate attribute term
	const float AttributeTerm { ApplyOperation(AttributeValue, AttributeCoefficient, AttributeOperation) };
	
	// Calculate level term
	const float LevelTerm { ApplyOperation(static_cast<float>(PlayerLevel), LevelCoefficient, LevelOperation) };

	// Combine everything based on the combine operation
	float Result { 0.f };
	
	switch (CombineOperation)
	{
	case EMMCCombineOperation::Add:
		// Base + AttributeTerm + LevelTerm
		Result = BaseValue + AttributeTerm + LevelTerm;
		break;
		
	case EMMCCombineOperation::Multiply:
		// (Base + AttributeTerm) * LevelTerm
		Result = (BaseValue + AttributeTerm) * LevelTerm;
		break;
		
	case EMMCCombineOperation::Mixed:
		// Base + (AttributeTerm * LevelTerm)
		Result = BaseValue + (AttributeTerm * LevelTerm);
		break;
		
	default:
		Result = BaseValue + AttributeTerm + LevelTerm;
		break;
	}

	return FMath::Max<float>(Result, 0.f);
}