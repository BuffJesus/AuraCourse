// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AuraExecCalc_TestMemeNumbers.generated.h"

/**
 * Simple test ExecCalc for testing meme damage numbers (69, 420, 3.14)
 * and different hit types (Normal, Blocked, Critical)
 */
UCLASS()
class AURA_API UAuraExecCalc_TestMemeNumbers : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UAuraExecCalc_TestMemeNumbers();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};