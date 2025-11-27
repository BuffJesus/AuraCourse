#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsNiceHit() const { return bIsNiceHit; }        // 69
	bool IsDankHit() const { return bIsDankHit; }        // 420
	bool IsPiHit() const { return bIsPiHit; }            // ~3.14
	
	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsNiceHit(const bool bInIsNiceHit) { bIsNiceHit = bInIsNiceHit; }
	void SetIsDankHit(const bool bInIsDankHit) { bIsDankHit = bInIsDankHit; }
	void SetIsPiHit(const bool bInIsPiHit) { bIsPiHit = bInIsPiHit; }
	
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FAuraGameplayEffectContext::StaticStruct();
	}
	
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext { new FAuraGameplayEffectContext() };
		*NewContext = *this;
		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
	
protected:
	UPROPERTY()
	bool bIsBlockedHit { false };
	
	UPROPERTY()
	bool bIsCriticalHit { false };
	
	UPROPERTY()
	bool bIsNiceHit { false };
	
	UPROPERTY()
	bool bIsDankHit { false };
	
	UPROPERTY()
	bool bIsPiHit { false };
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
