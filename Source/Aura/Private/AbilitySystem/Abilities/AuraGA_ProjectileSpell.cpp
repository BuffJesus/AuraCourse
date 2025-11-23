// Not Sure Yet


#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"

#include "Actors/AuraProjectile.h"
#include "Interaction/AuraCombatInterface.h"

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) { return; }

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor);
	if (!CombatInterface) { return; }

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FTransform SpawnTransform(SocketLocation);

	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass, 
		SpawnTransform, 
		AvatarActor, 
		Cast<APawn>(AvatarActor), 
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile) { Projectile->FinishSpawning(SpawnTransform); }
}
