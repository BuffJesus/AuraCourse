// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AuraInputConfig.h"
#include "AuraInputComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, 
		PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
	
	for (const auto& [InputAction, InputActionTag] : InputConfig->AbilityInputActions)
	{
		if (InputAction && InputActionTag.IsValid())
		{
			if (PressedFunc) { BindAction(InputActionTag, ETriggerEvent::Started, Object, PressedFunc, InputActionTag); }
			if (ReleasedFunc) { BindAction(InputActionTag, ETriggerEvent::Completed, Object, ReleasedFunc, InputActionTag); }
			if (HeldFunc) { BindAction(InputActionTag, ETriggerEvent::Triggered, Object, HeldFunc, InputActionTag); }
		}
	}
}
