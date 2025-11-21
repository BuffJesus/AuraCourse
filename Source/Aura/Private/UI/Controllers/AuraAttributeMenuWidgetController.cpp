// Not Sure Yet


#include "UI/Controllers/AuraAttributeMenuWidgetController.h"
#include "Tags/AuraTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraAttributeInfo.h"

void UAuraAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	
}

void UAuraAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS { CastChecked<UAuraAttributeSet>(AttributeSet) };
	
	check (AttributeInfo)
	
	FAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Aura::Attributes::Primary::Strength);
	Info.AttributeValue = AS->GetStrength();
	OnAttributeInfoChanged.Broadcast(Info);
}

