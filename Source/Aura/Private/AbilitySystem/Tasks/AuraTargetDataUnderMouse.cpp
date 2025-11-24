// Not Sure Yet


#include "AbilitySystem/Tasks/AuraTargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Player/AuraPlayerController.h"

UAuraTargetDataUnderMouse* UAuraTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UAuraTargetDataUnderMouse* MyObj { NewAbilityTask<UAuraTargetDataUnderMouse>(OwningAbility) };
	return MyObj;
}

void UAuraTargetDataUnderMouse::Activate()
{
	if (const bool bIsLocallyControlled { IsLocallyControlled() }) { SendCursorData(); }
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle { GetAbilitySpecHandle() };
		const FPredictionKey PredictionKey { GetActivationPredictionKey() };
		UAbilitySystemComponent* const ASC { AbilitySystemComponent.Get() };
		
		if (!ASC) { return; }
		
		ASC->AbilityTargetDataSetDelegate(SpecHandle, 
			PredictionKey).AddUObject(this, &UAuraTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey);
	}
}

void UAuraTargetDataUnderMouse::SendCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	FGameplayAbilityTargetData_SingleTargetHit* Data { new FGameplayAbilityTargetData_SingleTargetHit };
	FGameplayAbilityTargetDataHandle Handle;
	Handle.Add(Data);
	
	if (const AAuraPlayerController* PC { Cast<AAuraPlayerController>(Ability->GetCurrentActorInfo()->PlayerController.Get()) })
	{
		const FHitResult CursorHit = PC->GetCursorHit();
		Data->HitResult = CursorHit;
		
		AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), 
			GetActivationPredictionKey(), 
			Handle, 
			FGameplayTag(), 
			AbilitySystemComponent->ScopedPredictionKey);
		
		if (ShouldBroadcastAbilityTaskDelegates()) { ValidData.Broadcast(Handle); }
	}
}

void UAuraTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Handle,
	FGameplayTag ActivationTag)
{
	
}
