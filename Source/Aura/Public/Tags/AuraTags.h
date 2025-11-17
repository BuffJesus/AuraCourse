// Not Sure Yet

#pragma once

#include "NativeGameplayTags.h"

namespace Aura
{
	namespace Attributes
	{
		namespace Vital
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Health);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxHealth);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mana);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxMana);
		}
		namespace Primary
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Resilience);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Vigor);
		}
	}
	namespace Message
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthPotion);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaPotion);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthCrystal);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaCrystal);
	}
}

