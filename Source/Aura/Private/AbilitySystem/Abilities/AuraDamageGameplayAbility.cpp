// Not Sure Yet

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagsManager.h"
#include "Tags/AuraTags.h"

void UAuraDamageGameplayAbility::AssignDamageTypesToSpec(const FGameplayEffectSpecHandle& SpecHandle) const
{
	// If DamageTypes map is empty, use legacy behavior with all damage types
	if (DamageTypes.IsEmpty())
	{
		const float ScaledDamageValue { GetScaledDamage() };
		const FGameplayTagContainer AllDamageTypes { UGameplayTagsManager::Get().RequestGameplayTagChildren(Aura::Damage::Damage) };
		
		for (const FGameplayTag& DamageType : AllDamageTypes)
		{
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamageValue);
		}
	}
	else
	{
		// Use randomized damage ranges for each specified damage type
		for (const TTuple<FGameplayTag, FDamageRange>& Pair : DamageTypes)
		{
			const float ScaledMagnitudeMin { Pair.Value.DamageMin.GetValueAtLevel(GetAbilityLevel()) };
			const float ScaledMagnitudeMax { Pair.Value.DamageMax.GetValueAtLevel(GetAbilityLevel()) };
			
			// Roll random damage between min and max
			const float Magnitude { FMath::RandRange(ScaledMagnitudeMin, ScaledMagnitudeMax) };
			
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Magnitude);
		}
	}
}