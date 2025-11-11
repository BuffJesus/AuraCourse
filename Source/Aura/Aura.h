// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AttributeSet.h"
#include "Aura.generated.h"

constexpr int CustomDepthRed{250};

USTRUCT(BlueprintType)
struct FAuraAbilityReferences
{
	GENERATED_BODY()

	FAuraAbilityReferences()
		: AbilitySystemComponent(nullptr)
		, AttributeSet(nullptr)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|AbilitySystem")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|AbilitySystem")
	TObjectPtr<UAttributeSet> AttributeSet;

	// Helper to create components (call in constructor)
	void InitializeComponents(UObject* Owner, FName ASCName = "AbilitySystemComponent", FName AttrName = "AttributeSet")
	{
		AbilitySystemComponent = Owner->CreateDefaultSubobject<UAuraAbilitySystemComponent>(ASCName);
		AbilitySystemComponent->SetIsReplicated(true);
		
		AttributeSet = Owner->CreateDefaultSubobject<UAuraAttributeSet>(AttrName);
	}

	// Helper to initialize actor info (call in BeginPlay)
	void InitializeActorInfo(AActor* OwnerActor, AActor* AvatarActor)
	{
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->InitAbilityActorInfo(OwnerActor, AvatarActor);
		}
	}

	void SetReplicationMode(EGameplayEffectReplicationMode Mode)
	{
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->SetReplicationMode(Mode);
		}
	}
};