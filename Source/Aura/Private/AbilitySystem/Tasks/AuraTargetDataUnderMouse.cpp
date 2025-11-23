// Not Sure Yet


#include "AbilitySystem/Tasks/AuraTargetDataUnderMouse.h"

UAuraTargetDataUnderMouse* UAuraTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UAuraTargetDataUnderMouse* MyObj = NewAbilityTask<UAuraTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}
