// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "AttributeSet.h"
#include "AuraMMC_Base.generated.h"

/**
 * Defines how to apply a coefficient to a value
 */
UENUM(BlueprintType)
enum class EMMCOperationType : uint8
{
	Add      UMETA(DisplayName = "Add (Value + Coefficient)"),
	Multiply UMETA(DisplayName = "Multiply (Value * Coefficient)"),
	Power    UMETA(DisplayName = "Power (Value ^ Coefficient)"),
	None     UMETA(DisplayName = "None (Ignore this term)")
};

/**
 * Defines how to combine the attribute term and level term
 */
UENUM(BlueprintType)
enum class EMMCCombineOperation : uint8
{
	Add      UMETA(DisplayName = "Add (Base + Attr + Level)"),
	Multiply UMETA(DisplayName = "Multiply ((Base + Attr) * Level)"),
	Mixed    UMETA(DisplayName = "Mixed (Base + (Attr * Level))")
};

UCLASS(Blueprintable, BlueprintType)
class AURA_API UAuraMMC_Base : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UAuraMMC_Base();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	/** The attribute to capture and use in the calculation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Capture")
	FGameplayAttribute AttributeToCapture;

	/** Whether to snapshot the attribute value or use the current value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Capture", AdvancedDisplay)
	bool bSnapshot { false };

	/** Base value for the calculation (e.g., 80 for MaxHealth, 100 for MaxMana) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Formula")
	float BaseValue { 0.f };

	/** How to combine all the terms together */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Formula")
	EMMCCombineOperation CombineOperation { EMMCCombineOperation::Add };

	/** How to apply the attribute coefficient (Multiply, Add, Power, or None) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Term")
	EMMCOperationType AttributeOperation { EMMCOperationType::Multiply };

	/** Coefficient for the captured attribute (e.g., 2.5 for Vigor, 8 for Intelligence) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Term")
	float AttributeCoefficient { 1.f };

	/** How to apply the level coefficient (Multiply, Add, Power, or None) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Term")
	EMMCOperationType LevelOperation { EMMCOperationType::Multiply };

	/** Coefficient for the player level (e.g., 10 for MaxHealth, 15 for MaxMana) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Term")
	float LevelCoefficient { 1.f };

private:
	void SetupCaptureDefinition();
	
	float ApplyOperation(float Value, float Coefficient, EMMCOperationType Operation) const;
	
	FGameplayEffectAttributeCaptureDefinition AttributeDef;
};