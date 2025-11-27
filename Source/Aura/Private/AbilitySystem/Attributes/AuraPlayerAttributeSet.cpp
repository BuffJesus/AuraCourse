// Not Sure Yet

#include "AbilitySystem/Attributes/AuraPlayerAttributeSet.h"
#include "AbilitySystem/AuraAttributeSetMacros.h"
#include "Net/UnrealNetwork.h"

// When you add player-specific attributes, use the shared macros like this:
//
// void UAuraPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
//     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//     
//     REPLICATE_ATTRIBUTE(UAuraPlayerAttributeSet, Experience);
//     REPLICATE_ATTRIBUTE(UAuraPlayerAttributeSet, Gold);
// }
//
// IMPLEMENT_ATTRIBUTE_ONREP(UAuraPlayerAttributeSet, Experience)
// IMPLEMENT_ATTRIBUTE_ONREP(UAuraPlayerAttributeSet, Gold)