// Not Sure Yet

#include "AbilitySystem/AuraEnemyAttributeSet.h"
#include "AbilitySystem/AuraAttributeSetMacros.h"
#include "Net/UnrealNetwork.h"

// When you add enemy-specific attributes, use the shared macros like this:
//
// void UAuraEnemyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
// {
//     Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//     
//     REPLICATE_ATTRIBUTE(UAuraEnemyAttributeSet, AggroRange);
//     REPLICATE_ATTRIBUTE(UAuraEnemyAttributeSet, PatrolRadius);
// }
//
// IMPLEMENT_ATTRIBUTE_ONREP(UAuraEnemyAttributeSet, AggroRange)
// IMPLEMENT_ATTRIBUTE_ONREP(UAuraEnemyAttributeSet, PatrolRadius)