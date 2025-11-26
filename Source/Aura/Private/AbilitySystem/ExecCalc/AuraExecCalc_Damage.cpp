// Not Sure Yet


#include "AbilitySystem/ExecCalc/AuraExecCalc_Damage.h"
#include "AbilitySystemComponent.h"

UAuraExecCalc_Damage::UAuraExecCalc_Damage()
{
	
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
	
	const FGameplayEffectSpec Spec { ExecutionParams.GetOwningSpec() };
}
