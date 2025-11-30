#include "AbilitySystem/Abilities/AuraGA_MeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

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
	
	// Cache the target
	CachedTargetActor = TriggerEventData ? const_cast<AActor*>(Cast<AActor>(TriggerEventData->Target)) : nullptr;
	
	UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack - AvatarActor: %s, Target: %s"), 
		*AvatarActor->GetName(),
		CachedTargetActor ? *CachedTargetActor->GetName() : TEXT("NULL"));
	
	// Set up event listener for montage event
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		FGameplayEventMulticastDelegate& EventDelegate = ASC->GenericGameplayEventCallbacks.FindOrAdd(Aura::Event::Montage::MeleeAttack);
		EventDelegate.AddUObject(this, &UAuraGA_MeleeAttack::OnMeleeAttackEvent);
	}
	
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
	UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack::PerformMeleeAttack - Dealing damage!"));
	// TODO: Implement damage logic
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