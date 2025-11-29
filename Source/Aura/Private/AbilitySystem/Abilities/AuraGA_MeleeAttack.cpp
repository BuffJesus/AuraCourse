// Not Sure Yet


#include "AbilitySystem/Abilities/AuraGA_MeleeAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Interaction/AuraCombatInterface.h"

class UAbilityTask_PlayMontageAndWait;

void UAuraGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Get the avatar actor
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		UE_LOG(LogTemp, Error, TEXT("No AvatarActor found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// Get AttackMontage from the Combat Interface
	UAnimMontage* MontageToPlay = nullptr;
	if (const IAuraCombatInterface* CombatInterface { Cast<IAuraCombatInterface>(AvatarActor) })
	{
		MontageToPlay = CombatInterface->Execute_GetAttackMontage(AvatarActor);
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
		
		PlayMontageTask->OnCompleted.AddDynamic(this, &UAuraGA_MeleeAttack::OnMontageCompleted);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UAuraGA_MeleeAttack::OnMontageInterrupted);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UAuraGA_MeleeAttack::OnMontageCancelled);
		
		PlayMontageTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No HitReact montage found, ending ability"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UAuraGA_MeleeAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraGA_MeleeAttack::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAuraGA_MeleeAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
