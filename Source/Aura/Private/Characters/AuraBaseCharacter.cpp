// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AuraBaseCharacter.h"

#include "AbilitySystemComponent.h"

AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraBaseCharacter::InitializeAbilityActorInfo()
{
	
}

void AAuraBaseCharacter::InitializeDefaultAttributes() const
{
	for (const auto& Pair : DefaultAttributeEffectsMap)
	{
		if (Pair.Value)
		{
			ApplyDefaultGameplayEffect(Pair.Value, 1.f);
		}
	}
}

void AAuraBaseCharacter::ApplyDefaultGameplayEffect(const TSubclassOf<UGameplayEffect> EffectClass, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(EffectClass);
	FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, Level, EffectContext);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}
