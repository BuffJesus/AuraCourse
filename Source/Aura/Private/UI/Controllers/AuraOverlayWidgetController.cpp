// Not Sure Yet

#include "UI/Controllers/AuraOverlayWidgetController.h"
#include "Tags/AuraTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UAuraOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet { CastChecked<UAuraAttributeSet>(AttributeSet) };
	const float Health { AuraAttributeSet->GetHealth() };
	const float MaxHealth { AuraAttributeSet->GetMaxHealth() };
	const float Mana { AuraAttributeSet->GetMana() };
	const float MaxMana { AuraAttributeSet->GetMaxMana() };

	OnHealthChanged.Broadcast(Health);
	OnMaxHealthChanged.Broadcast(MaxHealth);
	OnManaChanged.Broadcast(Mana);
	OnMaxManaChanged.Broadcast(MaxMana);
}

void UAuraOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet { CastChecked<UAuraAttributeSet>(AttributeSet) };

	BindAttributeChangeDelegate(AuraAttributeSet->GetHealthAttribute(), OnHealthChanged);
	BindAttributeChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindAttributeChangeDelegate(AuraAttributeSet->GetManaAttribute(), OnManaChanged);
	BindAttributeChangeDelegate(AuraAttributeSet->GetMaxManaAttribute(), OnMaxManaChanged);

	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// Use the parent Message tag

				// Check if this tag matches "Aura.Message" or any child
				if (const FGameplayTag MessageTag { Aura::Message::Message }; Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row { GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag) };
					OnMessageWidgetRowChanged.Broadcast(*Row);
				}
			}
		}
	);
}
