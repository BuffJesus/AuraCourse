// Not Sure Yet


#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "Player/AuraPlayerState.h"
#include "UI/Controllers/AuraAttributeMenuWidgetController.h"
#include "UI/Controllers/AuraOverlayWidgetController.h"
#include "UI/Controllers/AuraWidgetController.h"
#include "UI/HUD/AuraHUD.h"

template <typename T>
T* UAuraAbilitySystemBPLibrary::GetWidgetController(const UObject* WorldContextObject,
													 T* (AAuraHUD::*GetControllerFunc)(const FWidgetControllerParams&))
{
	if (APlayerController* PC { WorldContextObject->GetWorld()->GetFirstPlayerController() })
	{
		if (AAuraHUD* AuraHUD { Cast<AAuraHUD>(PC->GetHUD()) })
		{
			AAuraPlayerState* PS { PC->GetPlayerState<AAuraPlayerState>() };
			UAbilitySystemComponent* ASC { PS->GetAbilitySystemComponent() };
			UAttributeSet* AS { PS->GetAttributeSet() };
			const FWidgetControllerParams Params(PC, PS, ASC, AS);
			
			// Call the member function pointer on the HUD
			return (AuraHUD->*GetControllerFunc)(Params);
		}
	}
	return nullptr;
}

UAuraOverlayWidgetController* UAuraAbilitySystemBPLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	return GetWidgetController<UAuraOverlayWidgetController>(WorldContextObject, &AAuraHUD::GetOverlayWidgetController);
}

UAuraAttributeMenuWidgetController* UAuraAbilitySystemBPLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	return GetWidgetController<UAuraAttributeMenuWidgetController>(WorldContextObject, &AAuraHUD::GetAttributeMenuWidgetController);
}