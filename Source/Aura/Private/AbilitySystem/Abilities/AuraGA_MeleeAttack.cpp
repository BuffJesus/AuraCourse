// Not Sure Yet

#include "AbilitySystem/Abilities/AuraGA_MeleeAttack.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

class UAbilityTask_PlayMontageAndWait;

UAuraGA_MeleeAttack::UAuraGA_MeleeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	// Set up the ability to trigger on gameplay event
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = Aura::Ability::Attack::Attack;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
	
	UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack::Constructor - Ability triggers configured for tag: %s"), 
		*FGameplayTag(Aura::Ability::Attack::Attack).ToString());
}

void UAuraGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, 
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("=== UAuraGA_MeleeAttack::ActivateAbility START ==="));
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	// Get the avatar actor
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack - No AvatarActor found!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack - AvatarActor: %s"), *AvatarActor->GetName());
	
	// Debug: Check if TriggerEventData exists
	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack - TriggerEventData is NULL! Motion warping will not work."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack - TriggerEventData exists"));
		
		// Get target from TriggerEventData
		AActor* TargetActor = const_cast<AActor*>(Cast<AActor>(TriggerEventData->Target));
		
		if (!TargetActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - No target in TriggerEventData!"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack - Target found: %s at location: %s"), 
				*TargetActor->GetName(), 
				*TargetActor->GetActorLocation().ToString());
			
			// Update facing target using motion warping
			if (IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
			{
				UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack - Calling UpdateFacingTarget for motion warping"));
				CombatInterface->Execute_UpdateFacingTarget(AvatarActor, TargetActor->GetActorLocation());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack - AvatarActor does not implement IAuraCombatInterface!"));
			}
		}
	}
	
	// Get AttackMontage from the Combat Interface
	UAnimMontage* MontageToPlay = nullptr;
	if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
	{
		MontageToPlay = CombatInterface->Execute_GetAttackMontage(AvatarActor);
	}
	
	// Play the montage and wait for it to complete
	if (MontageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - Playing Attack montage: %s"), *MontageToPlay->GetName());
		
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
		UE_LOG(LogTemp, Error, TEXT("UAuraGA_MeleeAttack - No Attack montage found, ending ability"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("=== UAuraGA_MeleeAttack::ActivateAbility END ==="));
}

void UAuraGA_MeleeAttack::OnMontageCompleted()
{
	UE_LOG(LogTemp, Log, TEXT("UAuraGA_MeleeAttack - Montage completed"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraGA_MeleeAttack::OnMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - Montage cancelled"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAuraGA_MeleeAttack::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("UAuraGA_MeleeAttack - Montage interrupted"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}