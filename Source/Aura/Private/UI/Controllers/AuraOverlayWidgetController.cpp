// Not Sure Yet

#include "UI/Controllers/AuraOverlayWidgetController.h"
#include "Tags/AuraTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UAuraOverlayWidgetController::BroadcastInitialValues()
{
	// Use typed getter - no cast needed!
	const UAuraAttributeSet* AuraAttributeSet { GetAuraAttributeSet() };
	
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UAuraOverlayWidgetController::BindCallbacksToDependencies()
{
	// Use typed getter - no cast needed!
	const UAuraAttributeSet* AuraAttributeSet { GetAuraAttributeSet() };

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
					if (Row) OnMessageWidgetRowChanged.Broadcast(*Row);
				}
			}
		}
	);
}