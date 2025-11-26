// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AuraExecCalc_Damage.generated.h"

class UAuraDamageCalcConfig;

UCLASS()
class AURA_API UAuraExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UAuraExecCalc_Damage();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, 
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	FORCEINLINE UAuraDamageCalcConfig* GetDamageConfig() const { return DamageConfig; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|DamageCalculation")
	UAuraDamageCalcConfig* DamageConfig;
};