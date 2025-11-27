// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AuraExecCalc_Potion.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AURA_API UAuraExecCalc_Potion : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UAuraExecCalc_Potion();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	/** Multiplier for level scaling (default: 5% per level = 0.05) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Potion", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LevelScalingMultiplier { 0.05f };

	/** Multiplier for Vigor bonus to health restoration (default: 0.5 HP per Vigor point) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Potion", meta = (ClampMin = "0.0"))
	float VigorBonusMultiplier { 0.5f };

	/** Multiplier for Intelligence bonus to mana restoration (default: 0.3 MP per Intelligence point) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Potion", meta = (ClampMin = "0.0"))
	float IntelligenceBonusMultiplier { 0.3f };

	/** Enable/disable level scaling */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Potion")
	bool bEnableLevelScaling { true };

	/** Enable/disable Vigor bonus for health potions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Potion")
	bool bEnableVigorBonus { true };

	/** Enable/disable Intelligence bonus for mana potions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Potion")
	bool bEnableIntelligenceBonus { true };
};