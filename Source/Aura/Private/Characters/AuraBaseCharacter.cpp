// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AuraBaseCharacter.h"

AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
