#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	// CRITICAL: First, serialize the parent class data
	// This handles all the standard FGameplayEffectContext data like instigator, ability, etc.
	uint32 RepBits { 0 };
	if (Ar.IsSaving())
	{
		// Build a bit mask of what data we need to serialize
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid())
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bIsBlockedHit)  // Our custom data
		{
			RepBits |= 1 << 7;
		}
		if (bIsCriticalHit)  // Our custom data
		{
			RepBits |= 1 << 8;
		}
	}

	// Serialize the bit mask
	Ar.SerializeBits(&RepBits, 9); // 9 bits total (7 base + 2 custom)

	// Serialize base class data based on the bit mask
	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		// Serialize the actor array
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		// Serialize hit result if present
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = MakeShared<FHitResult>();
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	
	// Serialize OUR custom data
	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockedHit;
	}
	if (RepBits & (1 << 8))
	{
		Ar << bIsCriticalHit;
	}

	// If we're loading and the bits weren't set, make sure bools are false
	if (Ar.IsLoading())
	{
		if (!(RepBits & (1 << 7)))
		{
			bIsBlockedHit = false;
		}
		if (!(RepBits & (1 << 8)))
		{
			bIsCriticalHit = false;
		}
	}

	bOutSuccess = true;
	return true;
}