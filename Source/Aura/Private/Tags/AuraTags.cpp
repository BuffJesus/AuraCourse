// Not Sure Yet


#include "Tags/AuraTags.h"

namespace Aura
{
	namespace Attributes
	{
		namespace Vital
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Health, "Aura.Attributes.Vital.Health", "Amount of damage an Actor can take");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mana, "Aura.Attributes.Vital.Mana", "Amount of mana an Actor can use");
		}
		namespace Primary
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Aura.Attributes.Primary.Strength", "Increases physical damage");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Aura.Attributes.Primary.Intelligence", "Increases magical damage");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Resilience, "Aura.Attributes.Primary.Resilience", "Increase armor and armor penetration");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Vigor, "Aura.Attributes.Primary.Vigor", "Increases max health");
		}
		namespace Secondary
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Armor, "Aura.Attributes.Secondary.Armor", "Reduces damage taken, improves block chance");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArmorPenetration, "Aura.Attributes.Secondary.ArmorPenetration", "Ignores a percentage of enemy armor, increases critical hit chance");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(BlockChance, "Aura.Attributes.Secondary.BlockChance", "Chance to reduce incoming damage by half");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(CriticalHitChance, "Aura.Attributes.Secondary.CriticalHitChance", "Chance to deal double damage plus critical hit bonus");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(CriticalHitDamage, "Aura.Attributes.Secondary.CriticalHitDamage", "Bonus damage added to critical hits");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(CriticalHitResistance, "Aura.Attributes.Secondary.CriticalHitResistance", "Reduces critical hit chance of attacking enemies");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthRegeneration, "Aura.Attributes.Secondary.HealthRegeneration", "Amount of health regenerated per second");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaRegeneration, "Aura.Attributes.Secondary.ManaRegeneration", "Amount of mana regenerated per second");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxHealth, "Aura.Attributes.Secondary.MaxHealth", "Maximum amount of health obtainable");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxMana, "Aura.Attributes.Secondary.MaxMana", "Maximum amount of mana obtainable");
		}
	}
	namespace Message
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message, "Aura.Message", "Message");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthPotion, "Aura.Message.HealthPotion", "Health potion");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaPotion, "Aura.Message.ManaPotion", "Mana potion");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthCrystal, "Aura.Message.HealthCrystal", "Health crystal");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaCrystal, "Aura.Message.ManaCrystal", "Mana crystal");
	}
}