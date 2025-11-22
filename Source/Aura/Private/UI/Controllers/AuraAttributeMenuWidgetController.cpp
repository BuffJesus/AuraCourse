// Not Sure Yet


#include "UI/Controllers/AuraAttributeMenuWidgetController.h"
#include "Tags/AuraTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraAttributeInfo.h"

void UAuraAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS { CastChecked<UAuraAttributeSet>(AttributeSet) };
	check (AttributeInfo);
	for (auto& Tag : AttributeInfo.Get()->AttributeInfo)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Tag.AttributeGetter).AddLambda(
			[this, Tag](const FOnAttributeChangeData& Data) { BroadcastAttributeInfo(Tag.AttributeTag);});
	}
}

void UAuraAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag) const
{
	// Getting info from DataAsset based on the gameplay tag match
	FAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
	// Set hidden attribute in the dataset
	Info.AttributeValue = Info.AttributeGetter.GetNumericValue(AttributeSet);
	// Broadcast for subscribers. IE attribute menu widget
	AttributeInfoDelegate.Broadcast(Info);
}

void UAuraAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS { CastChecked<UAuraAttributeSet>(AttributeSet) };
	check (AttributeInfo)
	for (auto& Tag : AttributeInfo.Get()->AttributeInfo) { BroadcastAttributeInfo(Tag.AttributeTag); }
}

