// Not Sure Yet


#include "UI/Controllers/AuraWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AuraAttributeSet.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& Params)
{
	PlayerController = Params.PlayerController;
	PlayerState = Params.PlayerState;
	AbilitySystemComponent = Params.AbilitySystemComponent;
	AttributeSet = Params.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
	// Override in child classes
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
	// Override in child classes
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraAbilitySystemComponent() const
{
	if (!CachedAuraASC)
	{
		CachedAuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return CachedAuraASC;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAttributeSet() const
{
	if (!CachedAuraAttributeSet)
	{
		CachedAuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	}
	return CachedAuraAttributeSet;
}