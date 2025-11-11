// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Aura/Aura.h"
#include "GameFramework/Character.h"
#include "AuraBaseCharacter.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

UCLASS(Abstract)
class AURA_API AAuraBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraBaseCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AbilityRefs.AttributeSet;}

protected:
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Combat")
	FName WeaponSocketName {FName("WeaponHandSocket")};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|AbilitySystem")
	FAuraAbilityReferences AbilityRefs; 
	
};
