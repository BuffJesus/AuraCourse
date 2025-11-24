// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interaction/AuraCombatInterface.h"
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

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Attributes")
	TArray<TSubclassOf<UGameplayEffect>> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Attributes")
	TArray<TSubclassOf<UGameplayEffect>> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Attributes")
	TArray<TSubclassOf<UGameplayEffect>> DefaultVitalAttributes;

	void InitializeDefaultAttributes() const;

	void AddCharacterAbilities();

private:
	void ApplyDefaultGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass, float Level = 1.f) const;

	UPROPERTY(EditAnywhere, Category = "Aura|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};