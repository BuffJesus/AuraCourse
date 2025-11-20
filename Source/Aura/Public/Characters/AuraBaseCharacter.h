// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AuraBaseCharacter.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
class UAttributeSet;

UCLASS(Abstract)
class AURA_API AAuraBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraBaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{ return AttributeSet; }

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Combat")
	FName WeaponSocketName { "WeaponHandSocket" };

protected:
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	// These will be set by derived classes
	UPROPERTY(VisibleAnywhere, Category = "Aura|GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitializeAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Attributes")
	TMap<FName, TSubclassOf<UGameplayEffect>> DefaultAttributeEffectsMap;

	void InitializeDefaultAttributes() const;

private:
	void ApplyDefaultGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass, float Level = 1.f) const;
};
