// Not Sure Yet


#include "Player/AuraPlayerState.h"

AAuraPlayerState::AAuraPlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilityRefs.InitializeComponents(this);
	AbilityRefs.SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilityRefs.AbilitySystemComponent;
}
