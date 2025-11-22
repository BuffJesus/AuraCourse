// Not Sure Yet


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FAuraInputAction& InputAction : AbilityInputActions)
	{
		if (InputAction.InputAction && InputAction.InputActionTag == InputTag) { return InputAction.InputAction; }
	}
	if (bLogNotFound) { UE_LOG(LogTemp, Warning, TEXT("InputAction for Tag %s not found!"), *InputTag.ToString()); }
	return nullptr;
}
