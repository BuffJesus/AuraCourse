// Not Sure Yet

#include "UI/Controllers/AuraWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& Params)
{
	PlayerController = Params.PlayerController;
	PlayerState = Params.PlayerState;
	AbilitySystemComponent = Params.AbilitySystemComponent;
	AttributeSet = Params.AttributeSet;
	
	// Clear cached typed AttributeSet when params change
	CachedAuraAttributeSet = nullptr;
}

void UAuraWidgetController::BroadcastInitialValues()
{
	// Override in child classes
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
	// Override in child classes
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAttributeSet() const
{
	if (!CachedAuraAttributeSet && AttributeSet)
	{
		// Cast once and cache - happens only once per widget controller
		CachedAuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	}
	return CachedAuraAttributeSet;
}