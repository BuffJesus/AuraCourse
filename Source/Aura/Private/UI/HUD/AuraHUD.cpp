// Not Sure Yet


#include "UI/HUD/AuraHUD.h"

#include "UI/Controllers/AuraAttributeMenuWidgetController.h"
#include "UI/Controllers/AuraOverlayWidgetController.h"
#include "UI/Widgets/AuraUserWidget.h"

UAuraOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& Params)
{
	return GetOrCreateWidgetController<UAuraOverlayWidgetController>(OverlayWidgetController, OverlayWidgetControllerClass, Params);
}

UAuraAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& Params)
{
	return GetOrCreateWidgetController<UAuraAttributeMenuWidgetController>(AttributeMenuWidgetController, AttributeMenuWidgetControllerClass, Params);
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAuraAbilitySystemComponent* ASC, UAuraAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass is not set, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass is not set, please fill out BP_AuraHUD"));
	
	UUserWidget* Widget { CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass) };
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UAuraOverlayWidgetController* WidgetController { GetOverlayWidgetController(WidgetControllerParams) };

	OverlayWidget->SetWidgetController(WidgetController);
	
	// Only broadcast initial values on the server
	// On clients, attribute replication will trigger the bound delegates
	if (GetOwningPlayerController()->HasAuthority())
	{
		WidgetController->BroadcastInitialValues();
	}
	
	Widget->AddToViewport();
}