// Not Sure Yet


#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"
#include "Actors/AuraProjectile.h"
#include "Interaction/AuraCombatInterface.h"

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
											   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
											   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	
	
}

void UAuraGA_ProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	const bool bIsLocallyControlled = GetActorInfo().IsLocallyControlled();

	IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation { (TargetLocation - SocketLocation).Rotation()  };
	Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// Server: Spawn authoritative projectile
	if (bIsServer)
	{
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//TODO: Give the Projectile a Gameplay Effect Spec for causing Damage.
		
		Projectile->FinishSpawning(SpawnTransform);
	}

	// Client: Spawn cosmetic projectile for prediction (if enabled and locally controlled)
	if (bUseCosmeticPrediction && bIsLocallyControlled && !bIsServer)
	{
		AAuraProjectile* CosmeticProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		CosmeticProjectile->bIsCosmetic = true;
		CosmeticProjectile->FinishSpawning(SpawnTransform);
	}
}