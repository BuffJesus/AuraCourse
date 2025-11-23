// Not Sure Yet


#include "UI/Controllers/AuraAttributeMenuWidgetController.h"
#include "Tags/AuraTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AuraAttributeInfo.h"
#include "Blueprint/UserWidget.h"
#include "Components/PanelWidget.h"

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
	
	for (const FAttributeInfo& Info : AttributeInfo->AttributeInfo)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Info.AttributeGetter)
			.AddLambda([this, Info](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Info);
			});
	}
}

void UAuraAttributeMenuWidgetController::SetAttributeTagsOnExistingRows(UUserWidget* ParentWidget)
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
		FString WidgetName { FString::Printf(TEXT("Row_%s"), *AttributeName) };
		
		// Search for widget by name in the parent widget's tree
		UWidget* RowWidget { ParentWidget->GetWidgetFromName(FName(*WidgetName)) };
		
		if (!RowWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find widget named: %s"), *WidgetName);
			continue;
		}
		
		// Cast to UserWidget to call Blueprint functions
		UUserWidget* RowUserWidget { Cast<UUserWidget>(RowWidget) };
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

void UAuraAttributeMenuWidgetController::PopulatePrimaryAttributeRows(UPanelWidget* Container, TSubclassOf<UUserWidget> RowWidgetClass)
{
	// Use Primary tag to filter
	const FGameplayTag PrimaryTag { Aura::Attributes::Primary::Primary };
	PopulateAttributeRowsInternal(Container, RowWidgetClass, PrimaryTag);
}

void UAuraAttributeMenuWidgetController::PopulateSecondaryAttributeRows(UPanelWidget* Container, TSubclassOf<UUserWidget> RowWidgetClass)
{
	// Use Secondary tag to filter
	const FGameplayTag SecondaryTag { Aura::Attributes::Secondary::Secondary };
	PopulateAttributeRowsInternal(Container, RowWidgetClass, SecondaryTag);
}

void UAuraAttributeMenuWidgetController::PopulateVitalAttributeRows(UPanelWidget* Container, TSubclassOf<UUserWidget> RowWidgetClass)
{
	// Use Vital tag to filter
	const FGameplayTag VitalTag { Aura::Attributes::Vital::Vital };
	PopulateAttributeRowsInternal(Container, RowWidgetClass, VitalTag);
}

void UAuraAttributeMenuWidgetController::PopulateAttributeRowsInternal(UPanelWidget* Container, 
	TSubclassOf<UUserWidget> RowWidgetClass, const FGameplayTag& FilterTag)
{
	if (!Container || !RowWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("PopulateAttributeRows: Container or RowWidgetClass is null"));
		return;
	}
	
	check(AttributeInfo);
	
	// Clear existing children
	Container->ClearChildren();
	
	int32 RowsAdded { 0 };
	
	// Create a row widget for each attribute in the DataAsset
	for (const FAttributeInfo& Info : AttributeInfo->AttributeInfo)
	{
		// If we have a filter tag, check if this attribute matches
		if (FilterTag.IsValid() && !Info.AttributeTag.MatchesTag(FilterTag))
		{
			continue; // Skip attributes that don't match the filter
		}
		
		// Create widget instance
		UUserWidget* RowWidget { CreateWidget<UUserWidget>(Container->GetWorld(), RowWidgetClass) };
		if (!RowWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create row widget for attribute: %s"), 
				*Info.AttributeTag.ToString());
			continue;
		}
		
		// Set the attribute tag on the widget
		// The widget Blueprint should have a function called "SetAttributeTag"
		UFunction* SetAttributeTagFunc { RowWidget->FindFunction(FName("SetAttributeTag")) };
		if (SetAttributeTagFunc)
		{
			struct FSetAttributeTagParams
			{
				FGameplayTag AttributeTag;
			};
			
			FSetAttributeTagParams Params;
			Params.AttributeTag = Info.AttributeTag;
			RowWidget->ProcessEvent(SetAttributeTagFunc, &Params);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Row widget does not have SetAttributeTag function"));
		}
		
		// Add to container
		Container->AddChild(RowWidget);
		RowsAdded++;
		
		UE_LOG(LogTemp, Log, TEXT("Added row for attribute: %s"), *Info.AttributeName.ToString());
	}
	
	const FString FilterName { FilterTag.IsValid() ? FilterTag.ToString() : TEXT("All") };
	UE_LOG(LogTemp, Log, TEXT("Populated %d attribute rows (Filter: %s)"), RowsAdded, *FilterName);
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