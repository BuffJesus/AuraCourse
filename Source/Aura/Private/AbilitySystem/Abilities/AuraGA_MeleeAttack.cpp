#include "AbilitySystem/Abilities/AuraGA_MeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

UAuraGA_MeleeAttack::UAuraGA_MeleeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	// Set up the ability to trigger on gameplay event
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = Aura::Ability::Attack::Attack;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}

void UAuraGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, 
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UE_LOG(LogTemp, Warning, TEXT("=== UAuraGA_MeleeAttack::ActivateAbility START ==="));
	
	// Get the avatar actor
        AActor* AvatarActor = GetAvatarActorFromActorInfo();
        if (!AvatarActor)
        {
                UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack - No AvatarActor found!"));
                EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
                return;
        }

        // Commit the ability so the server owns the activation and replicates it to clients
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
        {
                UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - CommitAbility failed"));
                EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
                return;
        }
	
	// Cache the target
	CachedTargetActor = TriggerEventData ? const_cast<AActor*>(Cast<AActor>(TriggerEventData->Target)) : nullptr;
	
	UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack - AvatarActor: %s, Target: %s"), 
		*AvatarActor->GetName(),
		CachedTargetActor ? *CachedTargetActor->GetName() : TEXT("NULL"));
	
        // Motion warping
        if (CachedTargetActor)
        {
		if (IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
		{
			CombatInterface->Execute_UpdateFacingTarget(AvatarActor, CachedTargetActor->GetActorLocation());
		}
	}
	
	// Get and play montage
	UAnimMontage* MontageToPlay = nullptr;
	if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
	{
		MontageToPlay = CombatInterface->Execute_GetAttackMontage(AvatarActor);
	}
	
        if (MontageToPlay)
        {
                // Commit the ability so the server owns the activation and replicates it to clients
                if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
                {
                        UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - CommitAbility failed"));
                        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
                        return;
                }

                // Set up event listener for montage event
                UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
                if (ASC)
                {
                        FGameplayEventMulticastDelegate& EventDelegate = ASC->GenericGameplayEventCallbacks.FindOrAdd(Aura::Event::Montage::MeleeAttack);
                        EventDelegate.AddUObject(this, &UAuraGA_MeleeAttack::OnMeleeAttackEvent);
                }

                UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - Playing montage: %s"), *MontageToPlay->GetName());
		
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			MontageToPlay,
			1.0f
		);
		
		PlayMontageTask->OnCompleted.AddDynamic(this, &UAuraGA_MeleeAttack::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UAuraGA_MeleeAttack::OnMontageInterrupted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UAuraGA_MeleeAttack::OnMontageCancelled);
		
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack - No Attack montage found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UAuraGA_MeleeAttack::OnMeleeAttackEvent(const FGameplayEventData* Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack::OnMeleeAttackEvent - MONTAGE EVENT FIRED!"));
	PerformMeleeAttack();
}

void UAuraGA_MeleeAttack::PerformMeleeAttack()
{
        UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Starting sphere trace!"));

	// Get the avatar actor
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - No AvatarActor!"));
		return;
	}
	
	// Get combat socket location
        const FVector CombatSocketLocation { GetCombatSocketLocation() };
        if (CombatSocketLocation.IsNearlyZero())
        {
                UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Invalid combat socket location!"));
                return;
        }
	
	// Set up sphere trace
	TArray<FHitResult> HitResults;
	FCollisionShape SphereShape { FCollisionShape::MakeSphere(SphereTraceRadius) };
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	
	// Perform sphere trace
	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - No World!"));
		return;
	}
	
	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		CombatSocketLocation,
		CombatSocketLocation,  // Start and end at same location for sphere overlap
		FQuat::Identity,
		ECC_Pawn,
		SphereShape,
		QueryParams
	);
	
	if (!bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Sphere trace found no targets"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Hit %d targets!"), HitResults.Num());
	
	// Get source ASC for creating damage specs
        if (!GetAbilitySystemComponentFromActorInfo())
        {
                UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - No source ASC!"));
                return;
        }

        FGameplayTagContainer SourceTags;
        if (const IGameplayTagAssetInterface* SourceTagInterface = Cast<IGameplayTagAssetInterface>(AvatarActor))
        {
                SourceTagInterface->GetOwnedGameplayTags(SourceTags);
        }

        // Process each hit
        for (const FHitResult& Hit : HitResults)
        {
                AActor* HitActor = Hit.GetActor();
                if (!IsValid(HitActor))
		{
			continue;
		}
		
		// Don't hit yourself
		if (HitActor == AvatarActor)
		{
                        continue;
                }

                // Skip friendlies based on entity gameplay tags
                if (const IGameplayTagAssetInterface* TargetTagInterface = Cast<IGameplayTagAssetInterface>(HitActor))
                {
                        FGameplayTagContainer TargetTags;
                        TargetTagInterface->GetOwnedGameplayTags(TargetTags);

                        const bool bSourceIsPlayer = SourceTags.HasTagExact(Aura::Entities::Player);
                        const bool bSourceIsEnemy = SourceTags.HasTagExact(Aura::Entities::Enemy);
                        const bool bTargetIsPlayer = TargetTags.HasTagExact(Aura::Entities::Player);
                        const bool bTargetIsEnemy = TargetTags.HasTagExact(Aura::Entities::Enemy);

                        if ((bSourceIsPlayer && bTargetIsPlayer) || (bSourceIsEnemy && bTargetIsEnemy))
                        {
                                UE_LOG(LogTemp, Verbose, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Skipping friendly: %s"), *HitActor->GetName());
                                continue;
                        }
                }

                UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Processing hit on: %s"), *HitActor->GetName());
		
                // Prevent friendly fire - only hit our current combat target
                // Get the combat target from our combat interface
                AActor* CombatTarget = nullptr;
                if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
                {
			CombatTarget = CombatInterface->GetCombatTarget();
		}
		
		// Skip if this isn't our combat target (prevents hitting other enemies)
		if (CombatTarget && HitActor != CombatTarget)
		{
			UE_LOG(LogTemp, Verbose, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Skipping non-target: %s (Target is: %s)"), 
				*HitActor->GetName(), *CombatTarget->GetName());
			continue;
                }

                // Create and apply damage effect
                const FGameplayEffectSpecHandle SpecHandle = MakeDamageEffectSpecHandle();
                if (!SpecHandle.IsValid())
                {
                        UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Failed to create damage spec for: %s"), *HitActor->GetName());
                        continue;
                }

                if (ApplyDamageEffectToTarget(HitActor, SpecHandle))
                {
                        UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Applied damage to: %s"), *HitActor->GetName());
                }
                else
                {
                        UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Target %s has no ASC!"), *HitActor->GetName());
                }
        }
}

void UAuraGA_MeleeAttack::OnMontageCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack - Montage completed"));
	CleanupEventListener();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraGA_MeleeAttack::OnMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - Montage cancelled"));
	CleanupEventListener();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAuraGA_MeleeAttack::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - Montage interrupted"));
	CleanupEventListener();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAuraGA_MeleeAttack::CleanupEventListener()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		FGameplayEventMulticastDelegate* EventDelegate = ASC->GenericGameplayEventCallbacks.Find(Aura::Event::Montage::MeleeAttack);
		if (EventDelegate)
		{
			EventDelegate->RemoveAll(this);
		}
	}
}