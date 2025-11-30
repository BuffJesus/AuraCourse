// Not Sure Yet


#include "Interaction/AuraCombatInterface.h"


// Add default functionality here for any IAuraCombatInterface functions that are not pure virtual.
int32 IAuraCombatInterface::GetCharacterLevel() const
{
	checkf(false, TEXT("GetCharacterLevel must be overridden by classes implementing AuraCombatInterface."));
	return 1;
}

FVector IAuraCombatInterface::GetCombatSocketLocation() const
{
        checkf(false, TEXT("GetCombatSocketLocation must be overridden by classes implementing AuraCombatInterface."));
        return FVector::ZeroVector;
}

FVector IAuraCombatInterface::GetCombatSocketLocationByTag(const FGameplayTag& SocketTag) const
{
        return GetCombatSocketLocation();
}

AActor* IAuraCombatInterface::GetCombatTarget() const
{
        return nullptr;
}

