// Not Sure Yet


#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"

#include "Kismet/KismetSystemLibrary.h"

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UKismetSystemLibrary::PrintString(this, FString("Projectile Spell Activated"));
}
