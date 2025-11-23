// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraGA_ProjectileSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGA_ProjectileSpell : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
