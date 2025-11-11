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
	AAuraPlayerCharacter();

	// Server only - called when controller possesses this pawn
	virtual void PossessedBy(AController* NewController) override;
	
	// Client only - called when PlayerState is replicated
	virtual void OnRep_PlayerState() override;
	

private:
	void InitAbilityActorInfo();

	UPROPERTY(EditAnywhere, Category = "Aura|Movement")
	float RotationRate{400.f};
};