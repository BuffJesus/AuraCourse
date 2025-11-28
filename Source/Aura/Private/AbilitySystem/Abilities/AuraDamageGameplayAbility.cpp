// Not Sure Yet

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "Tags/AuraTags.h"

void UAuraDamageGameplayAbility::AssignDamageTypesToSpec(const FGameplayEffectSpecHandle& SpecHandle) const
{
	const float ScaledDamageValue { GetScaledDamage() };
	
	// If DamageTypes is empty, use all children of Aura.Damage
	if (DamageTypes.IsEmpty())
	{
		const FGameplayTagContainer AllDamageTypes { UGameplayTagsManager::Get().RequestGameplayTagChildren(Aura::Damage::Damage) };
		for (const FGameplayTag& DamageType : AllDamageTypes)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamageValue);
		}
	}
	else
	{
		// Use specified damage types
		for (const FGameplayTag& DamageType : DamageTypes)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamageValue);
		}
	}
}