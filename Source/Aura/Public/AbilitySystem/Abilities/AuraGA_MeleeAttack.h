// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraGA_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGA_MeleeAttack : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAuraGA_MeleeAttack();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageCancelled();
	
	UFUNCTION()
	void OnMontageInterrupted();
};
