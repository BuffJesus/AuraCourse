
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "AuraCombatInterface.generated.h"

class AActor;
class UAnimMontage;

/**
 * Pairs an attack montage with its associated combat socket tag.
 * Used for characters with multiple attack animations that spawn effects from different sockets.
 */
USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage = nullptr;

	/** The socket tag indicating where combat effects should spawn (e.g., Weapon, LeftHand, RightHand) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketTag;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UAuraCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Combat interface for characters that participate in combat.
 */
class AURA_API IAuraCombatInterface
{
	GENERATED_BODY()

public:
	virtual int32 GetCharacterLevel() const;
	virtual FVector GetCombatSocketLocation() const;
	virtual FVector GetCombatSocketLocationByTag(const FGameplayTag& SocketTag) const;
	virtual AActor* GetCombatTarget() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	void UpdateFacingTarget(const FVector& Target);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	UAnimMontage* GetHitReactMontage() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	UAnimMontage* GetAttackMontage() const;

	/** Returns all available attack montages with their associated socket tags */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	TArray<FTaggedMontage> GetAttackMontages() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	AActor* GetAvatar();

	virtual void Die() = 0;
};