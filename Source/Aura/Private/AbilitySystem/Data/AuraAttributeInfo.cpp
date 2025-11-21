// Not Sure Yet


#include "AbilitySystem/Data/AuraAttributeInfo.h"
#include "Tags/AuraTags.h"

UAuraAttributeInfo::UAuraAttributeInfo()
{
	// Constructor
}

void UAuraAttributeInfo::PostInitProperties()
{
	Super::PostInitProperties();

	// Only initialize defaults if the array is empty (new asset creation)
	// This prevents overwriting user-edited data assets
	if (AttributeInfo.IsEmpty())
	{
		InitializeDefaultAttributeInfo();
	}
}

void UAuraAttributeInfo::InitializeDefaultAttributeInfo()
{
	// Macro to add attribute info with less boilerplate
	#define ADD_ATTRIBUTE_INFO(Namespace, AttributeName, DisplayName, Description) \
		AttributeInfo.Add(FAttributeInfo \
		{ \
			Aura::Attributes::Namespace::AttributeName, \
			FText::FromString(DisplayName), \
			FText::FromString(Description) \
		})

	// Primary Attributes
	ADD_ATTRIBUTE_INFO(Primary, Strength, "Strength", "Increases physical damage");
	ADD_ATTRIBUTE_INFO(Primary, Intelligence, "Intelligence", "Increases magical damage");
	ADD_ATTRIBUTE_INFO(Primary, Resilience, "Resilience", "Increase armor and armor penetration");
	ADD_ATTRIBUTE_INFO(Primary, Vigor, "Vigor", "Increases max health");

	// Secondary Attributes
	ADD_ATTRIBUTE_INFO(Secondary, Armor, "Armor", "Reduces damage taken, improves block chance");
	ADD_ATTRIBUTE_INFO(Secondary, ArmorPenetration, "Armor Penetration", "Ignores a percentage of enemy armor, increases critical hit chance");
	ADD_ATTRIBUTE_INFO(Secondary, BlockChance, "Block Chance", "Chance to reduce incoming damage by half");
	ADD_ATTRIBUTE_INFO(Secondary, CriticalHitChance, "Critical Hit Chance", "Chance to deal double damage plus critical hit bonus");
	ADD_ATTRIBUTE_INFO(Secondary, CriticalHitDamage, "Critical Hit Damage", "Bonus damage added to critical hits");
	ADD_ATTRIBUTE_INFO(Secondary, CriticalHitResistance, "Critical Hit Resistance", "Reduces critical hit chance of attacking enemies");
	ADD_ATTRIBUTE_INFO(Secondary, HealthRegeneration, "Health Regeneration", "Amount of health regenerated per second");
	ADD_ATTRIBUTE_INFO(Secondary, ManaRegeneration, "Mana Regeneration", "Amount of mana regenerated per second");
	ADD_ATTRIBUTE_INFO(Secondary, MaxHealth, "Max Health", "Maximum amount of health obtainable");
	ADD_ATTRIBUTE_INFO(Secondary, MaxMana, "Max Mana", "Maximum amount of mana obtainable");

	#undef ADD_ATTRIBUTE_INFO
}

FAttributeInfo UAuraAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, const bool bLogNotFound) const
{
	for (const FAttributeInfo& Info : AttributeInfo)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag)) { return Info; }
	}
	if (bLogNotFound) { UE_LOG(LogTemp,
		Warning,
		TEXT("Attribute Tag [%s] not found in AuraAttributeInfo [%s"),
		*AttributeTag.ToString(),*GetNameSafe(this)); }
	return FAttributeInfo();
}