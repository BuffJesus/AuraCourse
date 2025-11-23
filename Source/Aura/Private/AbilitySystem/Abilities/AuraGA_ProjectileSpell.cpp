// Not Sure Yet


#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/AuraProjectile.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, 
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!HasAuthority(&ActivationInfo)) { return; }
	
	UAbilityTask_WaitGameplayEvent* WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, EventTag, GetAvatarActorFromActorInfo(), false, true);
	
	if (WaitGameplayEventTask)
	{
		WaitGameplayEventTask->EventReceived.AddDynamic(this, &UAuraGA_ProjectileSpell::SpawnProjectile);
		WaitGameplayEventTask->ReadyForActivation();
	}
}

void UAuraGA_ProjectileSpell::SpawnProjectile(FGameplayEventData Payload)
{
	IAuraCombatInterface* CombatInterface { GetCombatInterfaceFromAvatar() };
	if (!CombatInterface) { return; }

	const FTransform SpawnTransform { GetProjectileSpawnTransform(CombatInterface) };

	if (AAuraProjectile* Projectile { CreateProjectile(SpawnTransform) })
	{
		Projectile->FinishSpawning(SpawnTransform);
	}
}

IAuraCombatInterface* UAuraGA_ProjectileSpell::GetCombatInterfaceFromAvatar() const
{
	AActor* AvatarActor { GetAvatarActorFromActorInfo() };
	return Cast<IAuraCombatInterface>(AvatarActor);
}

FTransform UAuraGA_ProjectileSpell::GetProjectileSpawnTransform(IAuraCombatInterface* CombatInterface) const
{
	const FVector SocketLocation { CombatInterface->GetCombatSocketLocation() };
	return FTransform(SocketLocation);
}

AAuraProjectile* UAuraGA_ProjectileSpell::CreateProjectile(const FTransform& SpawnTransform)
{
	AActor* AvatarActor { GetAvatarActorFromActorInfo() };
	
	return GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		AvatarActor,
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
}
