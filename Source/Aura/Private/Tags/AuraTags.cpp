// Not Sure Yet


#include "Tags/AuraTags.h"

namespace Aura
{
	namespace Attributes
	{
		namespace Vital
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Health, "Aura.Attributes.Vital.Health", "Amount of damage an Actor can take");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxHealth, "Aura.Attributes.Vital.MaxHealth", "Maximum amount of damage an Actor can take");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mana, "Aura.Attributes.Vital.Mana", "Amount of mana an Actor can use");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxMana, "Aura.Attributes.Vital.MaxMana", "Maximum amount of mana an Actor can use");
		}
		namespace Primary
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Aura.Attributes.Primary.Strength", "Increases physical damage");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Aura.Attributes.Primary.Intelligence", "Increases magical damage");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Resilience, "Aura.Attributes.Primary.Resilience", "Increase armor and armor penetration");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Vigor, "Aura.Attributes.Primary.Vigor", "Increases max health");
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
