// Not Sure Yet


#include "AbilitySystem/Data/AuraAttributeInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Tags/AuraTags.h"

#if WITH_EDITOR
#include "GameplayTagsManager.h"
#endif

FAttributeInfo UAuraAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, const bool bLogNotFound) const
{
	for (const FAttributeInfo& Info : AttributeInfo)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}
	
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attribute Tag [%s] not found in AuraAttributeInfo [%s]"),
			*AttributeTag.ToString(), *GetNameSafe(this));
	}
	
	return FAttributeInfo();
}

#if WITH_EDITOR
void UAuraAttributeInfo::PopulateDataAsset()
{
	// Clear existing data
	AttributeInfo.Empty();
	
	UGameplayTagsManager& TagManager { UGameplayTagsManager::Get() };
	
	// Get parent tags using native gameplay tags
	const FGameplayTag PrimaryTag { Aura::Attributes::Primary::Primary };
	const FGameplayTag SecondaryTag { Aura::Attributes::Secondary::Secondary };
	const FGameplayTag VitalTag { Aura::Attributes::Vital::Vital };
	
	// Get all gameplay tags
	FGameplayTagContainer AllTags;
	TagManager.RequestAllGameplayTags(AllTags, true);
	
	// Filter to only attribute tags (children of Primary, Secondary, or Vital)
	TArray<FGameplayTag> AttributeTags;
	for (const FGameplayTag& Tag : AllTags)
	{
		// Check if tag is a child of Primary, Secondary, or Vital (but not the parent tags themselves)
		if ((Tag.MatchesTag(PrimaryTag) && Tag != PrimaryTag) ||
		    (Tag.MatchesTag(SecondaryTag) && Tag != SecondaryTag) ||
		    (Tag.MatchesTag(VitalTag) && Tag != VitalTag))
		{
			AttributeTags.Add(Tag);
		}
	}
	
	// Get the AttributeSet class for property lookup
	UClass* AttributeSetClass { UAuraAttributeSet::StaticClass() };
	
	// Process each discovered tag
	for (const FGameplayTag& Tag : AttributeTags)
	{
		// Extract attribute name from tag (last part)
		FString TagName { Tag.GetTagName().ToString() };
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
		
		// Try to find the property in the AttributeSet using reflection
		FProperty* Property { AttributeSetClass->FindPropertyByName(FName(*AttributeName)) };
		if (!Property)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not find property for attribute: %s"), *AttributeName);
			continue;
		}
		
		// Verify it's an FGameplayAttributeData property
		FStructProperty* StructProperty { CastField<FStructProperty>(Property) };
		if (!StructProperty || StructProperty->Struct->GetFName() != FName("GameplayAttributeData"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Property %s is not a GameplayAttributeData"), *AttributeName);
			continue;
		}
		
		// Create the FGameplayAttribute from the property
		FGameplayAttribute GameplayAttribute { Property };
		
		// Format display name (insert spaces before capitals)
		FString FormattedName;
		for (int32 i = 0; i < AttributeName.Len(); ++i)
		{
			if (i > 0 && FChar::IsUpper(AttributeName[i]) && !FChar::IsUpper(AttributeName[i - 1]))
			{
				FormattedName += TEXT(" ");
			}
			FormattedName += AttributeName[i];
		}
		
		// Extract description from DevComment
		FString TagDescription;
		FName FirstTagSource;
		bool bIsTagExplicit { false };
		bool bIsRestrictedTag { false };
		bool bAllowNonRestrictedChildren { false };
		TagManager.GetTagEditorData(Tag.GetTagName(), TagDescription, FirstTagSource, 
			bIsTagExplicit, bIsRestrictedTag, bAllowNonRestrictedChildren);
		
		// Add the attribute info
		AttributeInfo.Add(FAttributeInfo
		{
			Tag,
			FText::FromString(FormattedName),
			FText::FromString(TagDescription.IsEmpty() ? TEXT("No description") : TagDescription),
			0.f,
			GameplayAttribute
		});
		
		UE_LOG(LogTemp, Log, TEXT("Added attribute: %s"), *FormattedName);
	}
	
	// Sort by tag name for consistency (Primary, Secondary, Vital order)
	AttributeInfo.Sort([](const FAttributeInfo& A, const FAttributeInfo& B)
	{
		return A.AttributeTag.GetTagName().ToString() < B.AttributeTag.GetTagName().ToString();
	});
	
	// Mark the asset as modified
	MarkPackageDirty();
	
	UE_LOG(LogTemp, Log, TEXT("DataAsset dynamically populated with %d attributes"), AttributeInfo.Num());
}
#endif