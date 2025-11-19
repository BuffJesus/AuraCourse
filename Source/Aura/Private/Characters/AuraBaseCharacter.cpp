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

void AAuraBaseCharacter::InitializePrimaryAttributes() const
{
	check (IsValid(GetAbilitySystemComponent()));
	check (DefaultPrimaryAttributes);
	const FGameplayEffectContextHandle EffectContext { GetAbilitySystemComponent()->MakeEffectContext() };
	const FGameplayEffectSpecHandle SpecHandle { GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, 1.f, EffectContext) };
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraBaseCharacter::InitializeVitalAttributes() const
{
	check (IsValid(GetAbilitySystemComponent()));
	check (DefaultVitalAttributes);
	const FGameplayEffectContextHandle EffectContext { GetAbilitySystemComponent()->MakeEffectContext() };
	const FGameplayEffectSpecHandle SpecHandle { GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultVitalAttributes, 1.f, EffectContext) };
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}
