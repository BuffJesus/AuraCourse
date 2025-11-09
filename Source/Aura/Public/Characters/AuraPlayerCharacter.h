// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "AuraPlayerCharacter.generated.h"

UCLASS()
class AURA_API AAuraPlayerCharacter : public AAuraBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
