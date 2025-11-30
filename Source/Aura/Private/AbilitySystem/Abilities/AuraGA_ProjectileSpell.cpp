// Not Sure Yet

#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Tasks/AuraTargetDataUnderMouse.h"
#include "Actors/AuraProjectile.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

UAuraGA_ProjectileSpell::UAuraGA_ProjectileSpell()
{
InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

// This ability runs on both client and server
// Client predicts the cast, server spawns the projectile
NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

// Allow AI to trigger this ability from gameplay events (e.g., AuraBTTask_Attack)
FAbilityTriggerData TriggerData;
TriggerData.TriggerTag = Aura::Ability::Attack::Attack;
TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
AbilityTriggers.Add(TriggerData);

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
UE_LOG(LogTemp, Log, TEXT("ProjectileSpell ActivateAbility | Avatar: %s | Owner: %s | TriggerEventTag: %s"),
AvatarActor ? *AvatarActor->GetName() : TEXT("None"),
ActorInfo && ActorInfo->OwnerActor.IsValid() ? *ActorInfo->OwnerActor->GetName() : TEXT("None"),
TriggerEventData ? *TriggerEventData->EventTag.ToString() : TEXT("None"));

if (!AvatarActor)
{
EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
return;
}

// Commit ability (checks cost and cooldown)
if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
{
EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
return;
}

// Step 1: Get target data (mouse cursor location for players, combat target for AI)
if (ActorInfo && ActorInfo->PlayerController.IsValid())
{
TargetDataTask = UAuraTargetDataUnderMouse::CreateTargetDataUnderMouse(this);
if (!TargetDataTask)
{
EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
return;
}

TargetDataTask->ValidData.AddDynamic(this, &UAuraGA_ProjectileSpell::OnTargetDataReceived);
TargetDataTask->ReadyForActivation();
}
else
{
// AI characters don't have mouse input; use their combat target instead
if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
{
if (AActor* CombatTarget = CombatInterface->GetCombatTarget())
{
UE_LOG(LogTemp, Log, TEXT("ProjectileSpell AI target acquired: %s"), *CombatTarget->GetName());

FGameplayAbilityTargetDataHandle DataHandle;
FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
TargetData->HitResult.Location = CombatTarget->GetActorLocation();
DataHandle.Add(TargetData);

OnTargetDataReceived(DataHandle);
return;
}

UE_LOG(LogTemp, Warning, TEXT("ProjectileSpell AI has no CombatTarget on %s"), *AvatarActor->GetName());
}

UE_LOG(LogTemp, Warning, TEXT("ProjectileSpell ending due to missing target data for %s"), *AvatarActor->GetName());
EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
}

void UAuraGA_ProjectileSpell::OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
// Extract target location from data handle
CachedTargetLocation = FVector::ZeroVector;
if (DataHandle.Data.Num() > 0)
{
const FGameplayAbilityTargetData* TargetData = DataHandle.Data[0].Get();
                if (TargetData)
                {
                        const FHitResult* HitResult = TargetData->GetHitResult();
                        if (HitResult)
                        {
                                CachedTargetLocation = HitResult->Location;
                        }
                }
}

if (CachedTargetLocation.IsNearlyZero())
{
if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(GetAvatarActorFromActorInfo()))
{
if (AActor* CombatTarget = CombatInterface->GetCombatTarget())
{
CachedTargetLocation = CombatTarget->GetActorLocation();
UE_LOG(LogTemp, Log, TEXT("ProjectileSpell fallback target location from combat target: %s"), *CachedTargetLocation.ToString());
}
}
}

UE_LOG(LogTemp, Log, TEXT("ProjectileSpell OnTargetDataReceived | CachedTargetLocation: %s | HasData: %s"),
*CachedTargetLocation.ToString(), DataHandle.Data.Num() > 0 ? TEXT("true") : TEXT("false"));

	// Step 2: Play montage (on both client and server for prediction)
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
	{
		if (UAnimMontage* MontageToPlay = CombatInterface->Execute_GetAttackMontage(AvatarActor))
		{
			// Update facing target before playing montage
			CombatInterface->Execute_UpdateFacingTarget(AvatarActor, CachedTargetLocation);
			
			PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
				this,
				NAME_None,
				MontageToPlay,
				1.0f
			);

			if (PlayMontageTask)
			{
				// OnCompleted: Montage finished playing successfully
				PlayMontageTask->OnCompleted.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCompleted);
				
				// OnInterrupted: Another montage interrupted this one
				PlayMontageTask->OnInterrupted.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCancelled);
				
				// OnCancelled: Montage was explicitly cancelled
				PlayMontageTask->OnCancelled.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCancelled);
				
				// NOTE: Do NOT bind OnBlendOut - it fires when blend out STARTS, not when montage ends
				// This was causing the ability to end early, allowing rapid re-activation

				PlayMontageTask->ReadyForActivation();
				
				// Step 3: Wait for gameplay event from AnimNotify
				// The AnimNotify in the montage will send the event tag (configured in MontageEventTag)
				if (MontageEventTag.IsValid())
				{
					EventWaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
						this,
						MontageEventTag,  // Use the configurable tag
						nullptr,          // Optional source (nullptr = any source)
						false,            // Only trigger once
						false             // Only match exact tag
					);

					if (EventWaitTask)
					{
						EventWaitTask->EventReceived.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageEventReceived);
						EventWaitTask->ReadyForActivation();
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("MontageEventTag is not set for %s. Projectile will not spawn."), *GetName());
				}
				
				return;
			}
		}
	}

	// If we couldn't play montage, end immediately
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraGA_ProjectileSpell::OnMontageEventReceived(FGameplayEventData Payload)
{
	// This is called when the AnimNotify sends the gameplay event
	// Now we can spawn the projectile at the exact frame in the animation
	SpawnProjectile(CachedTargetLocation, ProjectileSocketTag, bOverridePitch, PitchOverride);
}

void UAuraGA_ProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bShouldOverridePitch, float PitchOverrideValue)
{
	// Only spawn on server (projectiles are replicated)
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

	// Get the socket location using the provided SocketTag
	FVector SocketLocation { FVector::ZeroVector };
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
	{
		// Use the provided socket tag, or fall back to the default if not valid
		FGameplayTag ActualSocketTag = SocketTag.IsValid() ? SocketTag : ProjectileSocketTag;
		
		if (ActualSocketTag.IsValid())
		{
			SocketLocation = CombatInterface->GetCombatSocketLocationByTag(ActualSocketTag);
		}
		else
		{
			// Fallback to default combat socket
			SocketLocation = CombatInterface->GetCombatSocketLocation();
		}
	}

	if (SocketLocation.IsNearlyZero())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid combat socket location on %s when spawning projectile"), *GetName());
		return;
	}

	// Calculate rotation from socket to target
	FRotator Rotation { (ProjectileTargetLocation - SocketLocation).Rotation() };
	
	// Apply pitch override if requested (either from parameter or member variable)
	const bool bApplyPitchOverride = bShouldOverridePitch || this->bOverridePitch;
	if (bApplyPitchOverride)
	{
		Rotation.Pitch = bShouldOverridePitch ? PitchOverrideValue : this->PitchOverride;
	}

	FTransform SpawnTransform(Rotation.Quaternion(), SocketLocation);

	AAuraProjectile* Projectile { GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		AvatarActor,
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn) };

	// Create gameplay effect context
	FGameplayEffectContextHandle EffectContextHandle { SourceASC->MakeEffectContext() };
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);

	// Create damage effect spec with context
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
	if (ProjectileFlightCue.IsValid()) 
	{ 
		Projectile->FlightCueTag = ProjectileFlightCue; 
	}
	if (ProjectileImpactCue.IsValid()) 
	{ 
		Projectile->ImpactCueTag = ProjectileImpactCue; 
	}

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