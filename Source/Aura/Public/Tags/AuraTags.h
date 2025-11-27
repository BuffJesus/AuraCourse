// Not Sure Yet

#pragma once

#include "NativeGameplayTags.h"

namespace Aura
{
	namespace Attributes
	{
		namespace Primary
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Primary);
			
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Strength);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Intelligence);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Resilience);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Vigor);
		}
		namespace Secondary
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Secondary);
			
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Armor);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ArmorPenetration);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(BlockChance);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalHitChance);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalHitDamage);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(CriticalHitResistance);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthRegeneration);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaRegeneration);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxHealth);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxMana);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Luck);
		}
		namespace Vital
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Vital);
			
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Health);
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Mana);
		}
	}
	namespace Damage
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);
	}
	namespace Effects
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
	}
	namespace Event
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitReact);
		
		namespace Montage
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Montage);
			
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireBolt);
		}
	}
	namespace GameplayCue
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue);
    
		namespace Aura
		{
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(Aura);
        
			UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageText);
        
			namespace Projectile
			{
				UE_DECLARE_GAMEPLAY_TAG_EXTERN(Projectile);
				
				namespace Fireball
				{
					UE_DECLARE_GAMEPLAY_TAG_EXTERN(Fireball);
					
					UE_DECLARE_GAMEPLAY_TAG_EXTERN(Flight);
					UE_DECLARE_GAMEPLAY_TAG_EXTERN(Impact);
				}
			}
		}
	}
	namespace Input
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(LMB);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(RMB);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(One);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Two);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Three);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Four);
	}
	namespace Message
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message);
		
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthPotion);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaPotion);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthCrystal);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaCrystal);
	}
	namespace Potion
	{
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(Potion);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(HealthRestoration);
		UE_DECLARE_GAMEPLAY_TAG_EXTERN(ManaRestoration);
	}
}