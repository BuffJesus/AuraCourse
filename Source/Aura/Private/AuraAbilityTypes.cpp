#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
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
		if (bIsBlockedHit)
		{
			RepBits |= 1 << 7;
		}
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 8;
		}
		if (bIsNiceHit)  // NEW
		{
			RepBits |= 1 << 9;
		}
		if (bIsDankHit)  // NEW
		{
			RepBits |= 1 << 10;
		}
		if (bIsPiHit)  // NEW
		{
			RepBits |= 1 << 11;
		}
	}

	// Serialize the bit mask - NOW 12 BITS!
	Ar.SerializeBits(&RepBits, 12);

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
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
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
	if (RepBits & (1 << 9))
	{
		Ar << bIsNiceHit;
	}
	if (RepBits & (1 << 10))
	{
		Ar << bIsDankHit;
	}
	if (RepBits & (1 << 11))
	{
		Ar << bIsPiHit;
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
		if (!(RepBits & (1 << 9)))
		{
			bIsNiceHit = false;
		}
		if (!(RepBits & (1 << 10)))
		{
			bIsDankHit = false;
		}
		if (!(RepBits & (1 << 11)))
		{
			bIsPiHit = false;
		}
	}

	bOutSuccess = true;
	return true;
}