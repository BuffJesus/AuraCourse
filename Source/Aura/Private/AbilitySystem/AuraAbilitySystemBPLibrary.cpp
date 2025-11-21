// Not Sure Yet


#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "Player/AuraPlayerState.h"
#include "UI/Controllers/AuraWidgetController.h"
#include "UI/HUD/AuraHUD.h"

UAuraOverlayWidgetController* UAuraAbilitySystemBPLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC { WorldContextObject->GetWorld()->GetFirstPlayerController() })
	{
		if (AAuraHUD* AuraHUD { Cast<AAuraHUD>(PC->GetHUD()) })
		{
			AAuraPlayerState* PS { PC->GetPlayerState<AAuraPlayerState>() };
			UAbilitySystemComponent* ASC { PS->GetAbilitySystemComponent() };
			UAttributeSet* AS { PS->GetAttributeSet() };
			const FWidgetControllerParams Params(PC, PS, ASC, AS);
			return AuraHUD->GetOverlayWidgetController(Params);
		}
	}
	return nullptr;
}
