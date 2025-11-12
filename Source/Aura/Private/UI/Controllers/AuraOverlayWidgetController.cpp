// Not Sure Yet


#include "UI/Controllers/AuraOverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UAuraOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	const auto Health = AuraAttributeSet->GetHealth();
	const auto MaxHealth = AuraAttributeSet->GetMaxHealth();

	OnHealthChanged.Broadcast(Health);
	OnMaxHealthChanged.Broadcast(MaxHealth);
}
