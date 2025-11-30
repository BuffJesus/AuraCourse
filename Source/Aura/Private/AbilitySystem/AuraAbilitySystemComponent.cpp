// Not Sure Yet

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Tags/AuraTags.h"
#include "Algo/AnyOf.h"


void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	// CRITICAL FIX: With Mixed replication mode, abilities must be granted on both server AND client
	// Previously this had: if (!IsOwnerActorAuthoritative()) return;
	// This blocked clients from ever receiving abilities!
	
	// Server should grant abilities authoritatively
	// Clients should grant abilities locally for prediction/execution
	// Mixed mode doesn't replicate ability specs, only effects
	
	// Only the owner should grant abilities (server for all, autonomous proxy for self)
	if (!IsOwnerActorAuthoritative() && GetOwnerRole() != ROLE_AutonomousProxy)
	{
		return;
	}

	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		if (!AbilityClass)
		{
			continue;
		}

		const bool bAbilityAlreadyGranted = Algo::AnyOf(GetActivatableAbilities(), [AbilityClass](const FGameplayAbilitySpec& AbilitySpec)
		{
			return AbilitySpec.Ability && AbilitySpec.Ability->GetClass() == AbilityClass;
		});

		if (bAbilityAlreadyGranted)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec { FGameplayAbilitySpec(AbilityClass, 1) };
		if (const UAuraGameplayAbility* AuraAbility = AbilityClass->GetDefaultObject<UAuraGameplayAbility>())
		{
			FGameplayTagContainer& DynamicTags = AbilitySpec.GetDynamicSpecSourceTags();	// Reference, not copy!
			DynamicTags.AddTag(AuraAbility->StartupInputTag);
		}

		GiveAbility(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) { return; }
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (const FGameplayTagContainer& DynamicTags { AbilitySpec.GetDynamicSpecSourceTags() }; DynamicTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed( AbilitySpec);
			if (!AbilitySpec.IsActive()) { TryActivateAbility(AbilitySpec.Handle); }
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) { return; }
	
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (const FGameplayTagContainer& DynamicTags { AbilitySpec.GetDynamicSpecSourceTags() }; DynamicTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased( AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	EffectAssetTags.Broadcast(TagContainer);
}