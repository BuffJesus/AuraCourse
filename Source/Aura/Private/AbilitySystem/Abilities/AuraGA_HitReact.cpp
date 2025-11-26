#include "AbilitySystem/Abilities/AuraGA_HitReact.h"
#include "AbilitySystemComponent.h"
#include "Interaction/AuraCombatInterface.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/AuraTags.h"

UAuraGA_HitReact::UAuraGA_HitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	// Set up the ability to trigger on gameplay event
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = Aura::Event::HitReact;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
}


void UAuraGA_HitReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("UAuraGA_HitReact::ActivateAbility called!"));
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Apply the HitReact Gameplay Effect to self FIRST
	// This should apply the tags shown in your image (Aura.Effects.HitReact)
	if (HitReactEffect)
	{
		if (const FGameplayEffectSpecHandle SpecHandle { MakeOutgoingGameplayEffectSpec(HitReactEffect, GetAbilityLevel()) }; SpecHandle.IsValid())
		{
			// Use the parameters directly instead of Current* members
			const FActiveGameplayEffectHandle ActiveHandle { ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle) };
		}
	}
	
	// Get the avatar actor
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		UE_LOG(LogTemp, Error, TEXT("No AvatarActor found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Get HitReactMontage from the Combat Interface
	UAnimMontage* MontageToPlay = nullptr;
	if (const IAuraCombatInterface* CombatInterface { Cast<IAuraCombatInterface>(AvatarActor) })
	{
		MontageToPlay = CombatInterface->Execute_GetHitReactMontage(AvatarActor);
	}

	// Play the montage and wait for it to complete
	if (MontageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("Playing HitReact montage: %s"), *MontageToPlay->GetName());
		
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			MontageToPlay,
			1.0f
		);
		
		PlayMontageTask->OnCompleted.AddDynamic(this, &UAuraGA_HitReact::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UAuraGA_HitReact::OnMontageInterrupted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UAuraGA_HitReact::OnMontageCancelled);
		
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No HitReact montage found, ending ability"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UAuraGA_HitReact::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraGA_HitReact::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAuraGA_HitReact::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}