// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/Controllers/AuraWidgetController.h"
#include "AuraHUD.generated.h"

class UAuraAttributeMenuWidgetController;
class UAuraWidgetController;
class UAbilitySystemComponent;
class UAttributeSet;
struct FWidgetControllerParams;
class UAuraOverlayWidgetController;
class UAuraUserWidget;


UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	UAuraOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& Params);
	UAuraAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& Params);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UAuraOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAuraAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	
	template <typename T>
	T* GetOrCreateWidgetController(TObjectPtr<T>& WidgetController, 
	                                const TSubclassOf<UAuraWidgetController>& WidgetControllerClass, 
	                                const FWidgetControllerParams& Params);
};

template <typename T>
T* AAuraHUD::GetOrCreateWidgetController(TObjectPtr<T>& WidgetController,
                                          const TSubclassOf<UAuraWidgetController>& WidgetControllerClass,
                                          const FWidgetControllerParams& Params)
{
	// Validate the widget controller class is set
	checkf(WidgetControllerClass, TEXT("Widget Controller Class is not set, please fill out BP_AuraHUD"));

	// Create the controller if it doesn't exist
	if (WidgetController == nullptr)
	{
		WidgetController = NewObject<T>(this, WidgetControllerClass);
		WidgetController->SetWidgetControllerParams(Params);
		WidgetController->BindCallbacksToDependencies();
	}

	return WidgetController;
}