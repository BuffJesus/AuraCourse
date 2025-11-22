// Not Sure Yet


#include "UI/Controllers/AuraAttributeMenuWidgetController.h"
#include "Tags/AuraTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraAttributeInfo.h"

void UAuraAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS { CastChecked<UAuraAttributeSet>(AttributeSet) };
	check(AttributeInfo);
	
	// Loop through all attributes in the DataAsset and broadcast initial values
	for (const FAttributeInfo& Info : AttributeInfo->AttributeInfo)
	{
		BroadcastAttributeInfo(Info);
	}
}

void UAuraAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AS { CastChecked<UAuraAttributeSet>(AttributeSet) };
	check(AttributeInfo);
	
	// Loop through all attributes and bind to their value change delegates
	for (const FAttributeInfo& Info : AttributeInfo->AttributeInfo)
	{
		// Bind using AttributeGetter directly - capture Info by reference
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Info.AttributeGetter)
			.AddLambda([this, &Info](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Info);
			});
	}
}

void UAuraAttributeMenuWidgetController::BroadcastAttributeInfo(const FAttributeInfo& Info) const
{
	// Get the current value from AttributeSet using the AttributeGetter
	const UAuraAttributeSet* AS { CastChecked<UAuraAttributeSet>(AttributeSet) };
	
	// Create a copy of Info and update the AttributeValue
	FAttributeInfo NewInfo { Info };
	NewInfo.AttributeValue = Info.AttributeGetter.GetNumericValue(AS);
	
	// Broadcast the updated info to subscribers (attribute menu widget)
	AttributeInfoDelegate.Broadcast(NewInfo);
}