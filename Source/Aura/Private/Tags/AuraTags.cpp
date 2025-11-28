// Not Sure Yet


#include "Tags/AuraTags.h"

namespace Aura
{
	namespace Attributes
	{
		namespace Primary
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "Aura.Attributes.Primary", "Primary attribute");
			
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Strength, "Aura.Attributes.Primary.Strength", "Increases physical damage");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Intelligence, "Aura.Attributes.Primary.Intelligence", "Increases magical damage");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Resilience, "Aura.Attributes.Primary.Resilience", "Increase armor and armor penetration");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Vigor, "Aura.Attributes.Primary.Vigor", "Increases max health");
		}
		namespace Secondary
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "Aura.Attributes.Secondary", "Secondary attribute");
			
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
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Luck, "Aura.Attributes.Secondary.Luck", "Increases armor penetration, critical hit chance, and block chance");
		}
		namespace Resistance
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Resistance, "Aura.Attributes.Resistance", "Reduces damage taken from specific damage types");
			
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireResistance, "Aura.Attributes.Resistance.Fire", "Reduces fire damage taken");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(LightningResistance, "Aura.Attributes.Resistance.Lightning", "Reduces lightning damage taken");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(ArcaneResistance, "Aura.Attributes.Resistance.Arcane", "Reduces arcane damage taken");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(PhysicalResistance, "Aura.Attributes.Resistance.Physical", "Reduces physical damage taken");
		}
		namespace Vital
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Vital, "Aura.Attributes.Vital", "Vital attribute");
			
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Health, "Aura.Attributes.Vital.Health", "Amount of damage an Actor can take");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Mana, "Aura.Attributes.Vital.Mana", "Amount of mana an Actor can use");
		}
	}
	namespace Damage
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Aura.Damage", "Damage");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fire, "Aura.Damage.Fire", "Fire damage");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Lightning, "Aura.Damage.Lightning", "Lightning damage");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Arcane, "Aura.Damage.Arcane", "Arcane damage");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Physical, "Aura.Damage.Physical", "Physical damage");
	}
	namespace Effects
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effects, "Aura.Effects", "Effects");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "Aura.Effects.HitReact", "Hit reaction");
	}
	namespace Event
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event, "Aura.Event", "Event");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "Aura.Event.HitReact", "Hit reaction");
		namespace Montage
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Montage, "Aura.Event.Montage", "Montage");
			
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireBolt, "Aura.Event.Montage.FireBolt", "Fire bolt");
		}
		
	}
	namespace GameplayCue
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue, "GameplayCue", "Gameplay cue root");
    
		namespace Aura
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Aura, "GameplayCue.Aura", "Aura gameplay cues");
        
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(DamageText, "GameplayCue.Aura.DamageText", "Damage text display");
        
			namespace Projectile
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Projectile, "GameplayCue.Aura.Projectile", "Projectile gameplay cues");
				namespace Fireball
				{
					UE_DEFINE_GAMEPLAY_TAG_COMMENT(Fireball, "GameplayCue.Aura.Projectile.Fireball", "Fireball gameplay cues");
					
					UE_DEFINE_GAMEPLAY_TAG_COMMENT(Flight, "GameplayCue.Aura.Projectile.Fireball.Flight", "Fireball flight sound");
					UE_DEFINE_GAMEPLAY_TAG_COMMENT(Impact, "GameplayCue.Aura.Projectile.Fireball.Impact", "Fireball impact effects");
				}
			}
		}
	}
	namespace Input
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Input, "Aura.Input", "Input");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LMB, "Aura.Input.LMB", "Left mouse button");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RMB, "Aura.Input.RMB", "Right mouse button");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(One, "Aura.Input.1", "Button 1");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Two, "Aura.Input.2", "Button 2");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Three, "Aura.Input.3", "Button 3");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Four, "Aura.Input.4", "Button 4");
	}
	namespace Message
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message, "Aura.Message", "Message");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthPotion, "Aura.Message.HealthPotion", "Health potion");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaPotion, "Aura.Message.ManaPotion", "Mana potion");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthCrystal, "Aura.Message.HealthCrystal", "Health crystal");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaCrystal, "Aura.Message.ManaCrystal", "Mana crystal");
	}
	namespace Potion
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Potion, "Aura.Potion", "Potion");
		
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(HealthRestoration, "Aura.Potion.HealthRestoration", "Health restoration");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ManaRestoration, "Aura.Potion.ManaRestoration", "Mana restoration");
	}
}