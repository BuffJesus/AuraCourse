// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.h"
#include "GameplayTagContainer.h"
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

        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Damage")
        TMap<FGameplayTag, FDamageRange> DamageTypes { TMap<FGameplayTag, FDamageRange>() };

        /** Optional tag used to look up which combat socket this ability should use */
        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Combat")
        FGameplayTag CombatSocketTag;

        /** Helper function to get scaled damage at current ability level */
        UFUNCTION(BlueprintCallable, Category = "Aura|Damage")
        FORCEINLINE float GetScaledDamage() const
        {
                return Damage.GetValueAtLevel(GetAbilityLevel());
        }

        /** Assigns randomized damage values to all appropriate damage type tags on the spec */
        UFUNCTION(BlueprintCallable, Category = "Aura|Damage")
        void AssignDamageTypesToSpec(const FGameplayEffectSpecHandle& SpecHandle) const;

        /** Builds a damage effect spec using the owning ASC's default effect context */
        UFUNCTION(BlueprintCallable, Category = "Aura|Damage")
        FGameplayEffectSpecHandle MakeDamageEffectSpecHandle() const;

        /** Builds a damage effect spec using a caller-provided effect context */
        UFUNCTION(BlueprintCallable, Category = "Aura|Damage")
        FGameplayEffectSpecHandle MakeDamageEffectSpecHandleWithContext(const FGameplayEffectContextHandle& EffectContextHandle) const;

        /** Applies a pre-built damage spec to a target actor if it has an ASC */
        UFUNCTION(BlueprintCallable, Category = "Aura|Damage")
        bool ApplyDamageEffectToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle) const;

        /** Returns the combat socket location for this ability, falling back to the default socket */
        UFUNCTION(BlueprintCallable, Category = "Aura|Combat")
        FVector GetCombatSocketLocation() const;
};