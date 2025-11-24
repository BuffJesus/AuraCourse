// Not Sure Yet


#include "AbilitySystem/Tasks/AuraTargetDataUnderMouse.h"

#include "Player/AuraPlayerController.h"

UAuraTargetDataUnderMouse* UAuraTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UAuraTargetDataUnderMouse* MyObj = NewAbilityTask<UAuraTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UAuraTargetDataUnderMouse::Activate()
{
	if (const AAuraPlayerController* PC { Cast<AAuraPlayerController>(Ability->GetCurrentActorInfo()->PlayerController.Get()) })
	{
		const FHitResult CursorHit { PC->GetCursorHit() };
		ValidData.Broadcast(CursorHit.Location);
	}
}
