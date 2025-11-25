// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AuraCollisionChannels.h"
#include "Interaction/AuraCombatInterface.h"
#include "AbilitySystem/Data/AuraCharacterClassInfo.h"
#include "AuraBaseCharacter.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class UMotionWarpingComponent;

UCLASS(Abstract)
class AURA_API AAuraBaseCharacter : public ACharacter, public IAbilitySystemInterface, public IAuraCombatInterface
{
	GENERATED_BODY()

public:
	AAuraBaseCharacter();

	// Interface requirement - returns base type
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// Typed getter - returns UAuraAbilitySystemComponent (no cast needed!)
	FORCEINLINE UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	// Typed getter - returns UAuraAttributeSet (no cast needed!)
	FORCEINLINE UAuraAttributeSet* GetAuraAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName WeaponSocketName { "WeaponHandSocket" };
	
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName WeaponTipSocketName { "WeaponTipSocket" };
	
	virtual FVector GetCombatSocketLocation() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Aura|Combat")
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }

protected:
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	// Store as typed pointer - no casting needed when accessing!
	UPROPERTY(VisibleAnywhere, Category = "Aura|GAS")
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Aura|Combat")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	// Store as typed pointer - no casting needed when accessing!
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;

	virtual void InitializeAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Character Class Defaults")
	ECharacterClass CharacterClass { ECharacterClass::DefaultClass };

	void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();
	
	// Implement the interface function - use _Implementation for BlueprintNativeEvent
	virtual void UpdateFacingTarget_Implementation(const FVector& Target) override;

	/** Name of the warp target used for motion warping */
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Combat")
	FName FacingTargetWarpName = "FacingTarget";

private:
	void ApplyGameplayEffectClassToSelf(TSubclassOf<UGameplayEffect> EffectClass, float Level = 1.f) const;

	UPROPERTY(EditAnywhere, Category = "Aura|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};