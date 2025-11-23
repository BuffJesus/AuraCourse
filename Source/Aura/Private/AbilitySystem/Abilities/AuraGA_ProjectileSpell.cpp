// AuraGA_ProjectileSpell.cpp
#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Actors/AuraProjectile.h"
#include "Interaction/AuraCombatInterface.h"

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, 
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Create and bind montage task
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("PlayAttackMontage"),
		AttackMontage
	);

	// Create and bind event task
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		EventTag
	);

	if (MontageTask && EventTask)
	{
		// Bind montage delegates
		MontageTask->OnCompleted.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCancelled);

		// Bind event delegate
		EventTask->EventReceived.AddDynamic(this, &UAuraGA_ProjectileSpell::OnEventReceived);

		// Activate both tasks
		MontageTask->ReadyForActivation();
		EventTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UAuraGA_ProjectileSpell::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraGA_ProjectileSpell::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAuraGA_ProjectileSpell::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAuraGA_ProjectileSpell::OnEventReceived(FGameplayEventData Payload)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("OnEventReceived - Authority: %s"), bIsServer ? TEXT("SERVER") : TEXT("CLIENT"));
	
	// Only spawn projectile on server
	if (bIsServer)
	{
		SpawnProjectile();
	}
}

void UAuraGA_ProjectileSpell::SpawnProjectile()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnProjectile called on SERVER"));
	
	IAuraCombatInterface* CombatInterface { GetCombatInterfaceFromAvatar() };
	if (!CombatInterface) 
	{ 
		UE_LOG(LogTemp, Error, TEXT("CombatInterface is null!"));
		return; 
	}

	const FTransform SpawnTransform { GetProjectileSpawnTransform(CombatInterface) };
	
	UE_LOG(LogTemp, Warning, TEXT("Spawning projectile at location: %s"), *SpawnTransform.GetLocation().ToString());

	if (AAuraProjectile* Projectile { CreateProjectile(SpawnTransform) })
	{
		Projectile->FinishSpawning(SpawnTransform);
		UE_LOG(LogTemp, Warning, TEXT("Projectile spawned successfully: %s"), *Projectile->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create projectile!"));
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
	
	// Get the avatar's rotation so projectile travels in the right direction
	const AActor* AvatarActor { GetAvatarActorFromActorInfo() };
	const FRotator AvatarRotation { AvatarActor->GetActorRotation() };
	
	// Return transform with both location AND rotation
	return FTransform(AvatarRotation, SocketLocation);
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