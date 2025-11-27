// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AuraExecCalc_Potion.generated.h"

/**
 * Execution Calculation for Health and Mana Potions
 * Handles restoration of vital attributes with optional level scaling
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API UAuraExecCalc_Potion : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UAuraExecCalc_Potion();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};