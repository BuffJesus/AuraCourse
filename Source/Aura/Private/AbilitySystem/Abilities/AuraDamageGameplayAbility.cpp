// Not Sure Yet

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagsManager.h"
#include "Interaction/AuraCombatInterface.h"
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

FGameplayEffectSpecHandle UAuraDamageGameplayAbility::MakeDamageEffectSpecHandle() const
{
        const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
        AssignDamageTypesToSpec(SpecHandle);
        return SpecHandle;
}

FGameplayEffectSpecHandle UAuraDamageGameplayAbility::MakeDamageEffectSpecHandle(const FGameplayEffectContextHandle& EffectContextHandle) const
{
        const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
        if (!IsValid(SourceASC))
        {
                return FGameplayEffectSpecHandle();
        }

        const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
        AssignDamageTypesToSpec(SpecHandle);
        return SpecHandle;
}

bool UAuraDamageGameplayAbility::ApplyDamageEffectToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& SpecHandle) const
{
        if (!TargetActor || !SpecHandle.IsValid())
        {
                return false;
        }

        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
        if (!TargetASC)
        {
                if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor))
                {
                        TargetASC = ASI->GetAbilitySystemComponent();
                }
        }

        if (!TargetASC)
        {
                return false;
        }

        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        FGameplayEventData EventData;
        EventData.Instigator = GetAvatarActorFromActorInfo();
        EventData.Target = TargetActor;
        TargetASC->HandleGameplayEvent(Aura::Event::HitReact, &EventData);

        return true;
}

FVector UAuraDamageGameplayAbility::GetCombatSocketLocation() const
{
        const AActor* AvatarActor = GetAvatarActorFromActorInfo();
        if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
        {
                if (CombatSocketTag.IsValid())
                {
                        return CombatInterface->GetCombatSocketLocationByTag(CombatSocketTag);
                }

                return CombatInterface->GetCombatSocketLocation();
        }

        return FVector::ZeroVector;
}