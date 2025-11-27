// Not Sure Yet

#include "UI/Controllers/AuraAttributeMenuWidgetController.h"
#include "AbilitySystem/Attributes/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraAttributeInfo.h"
#include "UI/Widgets/AuraUserWidget.h"

void UAuraAttributeMenuWidgetController::BroadcastInitialValues()
{
	// Use typed getter - no cast needed!
	check(AttributeInfo);
	
	// Loop through all attributes in the DataAsset and broadcast initial values
	for (const FAttributeInfo& Info : AttributeInfo->AttributeInfo)
	{
		BroadcastAttributeInfo(Info);
	}
}

void UAuraAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	// Use typed getter - no cast needed!
	check(AttributeInfo);
	
	// Bind to attribute change delegates for all attributes
	for (const FAttributeInfo& Info : AttributeInfo->AttributeInfo)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Info.AttributeGetter)
			.AddLambda([this, Info](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Info);
			});
	}
}

void UAuraAttributeMenuWidgetController::SetAttributeTagsOnExistingRows(UAuraUserWidget* ParentWidget)
{
	if (!ParentWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetAttributeTagsOnExistingRows: ParentWidget is null"));
		return;
	}
	
	check(AttributeInfo);
	
	int32 TagsSet { 0 };
	
	// Iterate through all attributes in the DataAsset
	for (const FAttributeInfo& Info : AttributeInfo->AttributeInfo)
	{
		// Extract attribute name from tag (e.g., "Strength" from "Aura.Attributes.Primary.Strength")
		FString TagName { Info.AttributeTag.GetTagName().ToString() };
		const int32 LastDotIndex { TagName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd) };
		FString AttributeName;
		
		if (LastDotIndex != INDEX_NONE)
		{
			AttributeName = TagName.RightChop(LastDotIndex + 1);
		}
		else
		{
			AttributeName = TagName;
		}
		
		// Construct the expected widget name (e.g., "Row_Strength")
		const FString WidgetName { FString::Printf(TEXT("Row_%s"), *AttributeName) };
		
		// Search for widget by name in the parent widget's tree
		UWidget* RowWidget { ParentWidget->GetWidgetFromName(FName(*WidgetName)) };
		
		if (!RowWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find widget named: %s"), *WidgetName);
			continue;
		}
		
		// Cast to UserWidget to call Blueprint functions
		UAuraUserWidget* RowUserWidget { Cast<UAuraUserWidget>(RowWidget) };
		if (!RowUserWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget %s is not a UserWidget"), *WidgetName);
			continue;
		}
		
		// Call SetAttributeTag function on the widget
		UFunction* SetAttributeTagFunc { RowUserWidget->FindFunction(FName("SetAttributeTag")) };
		if (SetAttributeTagFunc)
		{
			struct FSetAttributeTagParams
			{
				FGameplayTag AttributeTag;
			};
			
			FSetAttributeTagParams Params;
			Params.AttributeTag = Info.AttributeTag;
			RowUserWidget->ProcessEvent(SetAttributeTagFunc, &Params);
			
			TagsSet++;
			UE_LOG(LogTemp, Log, TEXT("Set tag on widget: %s → %s"), *WidgetName, *Info.AttributeTag.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget %s does not have SetAttributeTag function"), *WidgetName);
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Set attribute tags on %d existing row widgets"), TagsSet);
}

void UAuraAttributeMenuWidgetController::BroadcastAttributeInfo(const FAttributeInfo& Info) const
{
	// Use typed getter - no cast needed!
	const UAuraAttributeSet* AS { GetAuraAttributeSet() };
	
	// Create a copy of Info and update the AttributeValue
	FAttributeInfo NewInfo { Info };
	NewInfo.AttributeValue = Info.AttributeGetter.GetNumericValue(AS);
	
	// Broadcast the updated info to subscribers (attribute menu widget)
	AttributeInfoDelegate.Broadcast(NewInfo);
}