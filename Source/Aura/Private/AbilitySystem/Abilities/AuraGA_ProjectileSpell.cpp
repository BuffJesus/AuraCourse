// Not Sure Yet

#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/AuraProjectile.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

UAuraGA_ProjectileSpell::UAuraGA_ProjectileSpell()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	// Block re-activation while this ability is active (prevents machine-gun casting)
	ActivationOwnedTags.AddTag(Aura::Ability::State::Casting);
	ActivationBlockedTags.AddTag(Aura::Ability::State::Casting);
}

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, 
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
	{
		if (UAnimMontage* MontageToPlay = CombatInterface->Execute_GetAttackMontage(AvatarActor))
		{
			UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				MontageToPlay,
				1.0f
			);

			if (MontageTask)
			{
				MontageTask->OnCompleted.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCompleted);
				MontageTask->OnInterrupted.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCancelled);
				MontageTask->OnCancelled.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCancelled);
				MontageTask->OnBlendOut.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCompleted);

				MontageTask->ReadyForActivation();
				return;
			}
		}
	}

	// If no montage is available, end immediately to avoid getting stuck in an active state
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UAuraGA_ProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	const bool bIsServer { GetAvatarActorFromActorInfo()->HasAuthority() };
	if (!bIsServer) return;

	const UAbilitySystemComponent* SourceASC { UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()) };
	if (!IsValid(SourceASC))
	{
		UE_LOG(LogTemp, Error, TEXT("No AbilitySystemComponent found for %s when spawning projectile"), *GetName());
		return;
	}

	if (!ProjectileClass || !DamageEffectClass)
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileClass or DamageEffectClass not set on %s"), *GetName());
		return;
	}

	const FVector SocketLocation { GetCombatSocketLocation() };
	if (SocketLocation.IsNearlyZero())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid combat socket location on %s when spawning projectile"), *GetName());
		return;
	}

	const FRotator Rotation { (TargetLocation - SocketLocation).Rotation() };

	FTransform SpawnTransform(Rotation.Quaternion(), SocketLocation);

	AActor* AvatarActor { GetAvatarActorFromActorInfo() };

	AAuraProjectile* Projectile { GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		AvatarActor,
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn) };

	FGameplayEffectContextHandle EffectContextHandle { SourceASC->MakeEffectContext() };
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = TargetLocation;
	EffectContextHandle.AddHitResult(HitResult);

	const FGameplayEffectSpecHandle SpecHandle { MakeDamageEffectSpecHandleWithContext(EffectContextHandle) };
	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create damage spec for %s"), *GetName());
		Projectile->Destroy();
		return;
	}

	Projectile->DamageEffectSpecHandle = SpecHandle;

	// Only override cue tags if ability has them configured
	// Otherwise, projectile keeps its Blueprint defaults
	if (ProjectileFlightCue.IsValid()) { Projectile->FlightCueTag = ProjectileFlightCue; }
	if (ProjectileImpactCue.IsValid()) { Projectile->ImpactCueTag = ProjectileImpactCue; }

	Projectile->FinishSpawning(SpawnTransform);
}

void UAuraGA_ProjectileSpell::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraGA_ProjectileSpell::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}