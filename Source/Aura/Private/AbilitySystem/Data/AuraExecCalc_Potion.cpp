// Not Sure Yet

#include "AbilitySystem/Data/AuraExecCalc_Potion.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Tags/AuraTags.h"

UAuraExecCalc_Potion::UAuraExecCalc_Potion()
{
	// No attributes need to be captured - we just use SetByCaller values
}

void UAuraExecCalc_Potion::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Get healing amounts from SetByCaller
	const float HealthRestoration = Spec.GetSetByCallerMagnitude(Aura::Potion::HealthRestoration, false, 0.f);
	const float ManaRestoration = Spec.GetSetByCallerMagnitude(Aura::Potion::ManaRestoration, false, 0.f);

	// Apply health restoration if specified
	if (HealthRestoration > 0.f)
	{
		const FGameplayModifierEvaluatedData HealthData(
			UAuraAttributeSet::GetHealthAttribute(), 
			EGameplayModOp::Additive, 
			HealthRestoration
		);
		OutExecutionOutput.AddOutputModifier(HealthData);
		
		UE_LOG(LogTemp, Log, TEXT("Potion restored %.2f Health"), HealthRestoration);
	}

	// Apply mana restoration if specified
	if (ManaRestoration > 0.f)
	{
		const FGameplayModifierEvaluatedData ManaData(
			UAuraAttributeSet::GetManaAttribute(), 
			EGameplayModOp::Additive, 
			ManaRestoration
		);
		OutExecutionOutput.AddOutputModifier(ManaData);
		
		UE_LOG(LogTemp, Log, TEXT("Potion restored %.2f Mana"), ManaRestoration);
	}
}