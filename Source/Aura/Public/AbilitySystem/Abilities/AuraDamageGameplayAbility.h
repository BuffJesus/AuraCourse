// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "AuraDamageGameplayAbility.generated.h"

UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Damage")
	FScalableFloat Damage { 0.f };
	
	/** 
	 * Damage types to apply. If empty, will apply damage to ALL damage type tags.
	 * Specify specific types (e.g., only Fire) to restrict damage types.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Damage")
	FGameplayTagContainer DamageTypes { FGameplayTagContainer() };
	
	/** Helper function to get scaled damage at current ability level */
	UFUNCTION(BlueprintCallable, Category = "Aura|Damage")
	FORCEINLINE float GetScaledDamage() const
	{
		return Damage.GetValueAtLevel(GetAbilityLevel());
	}
	
	/** Assigns damage values to all appropriate damage type tags on the spec */
	UFUNCTION(BlueprintCallable, Category = "Aura|Damage")
	void AssignDamageTypesToSpec(const FGameplayEffectSpecHandle& SpecHandle) const;
};