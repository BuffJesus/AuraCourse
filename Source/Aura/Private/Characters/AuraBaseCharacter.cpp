// Fill out your copyright notice in the Description page of Project Settings.


#include "Aura/Public/Characters/AuraBaseCharacter.h"

#include "IO/IoStoreOnDemand.h"


AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}
