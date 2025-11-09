// Fill out your copyright notice in the Description page of Project Settings.


#include "Aura/Public/Characters/AuraBaseCharacter.h"


// Sets default values
AAuraBaseCharacter::AAuraBaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAuraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAuraBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAuraBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

