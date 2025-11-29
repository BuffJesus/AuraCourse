// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "AuraGCN_ProjectileFlight.generated.h"

/**
 * Duration-based Gameplay Cue for projectile flight sound
 * Uses Add/Remove to loop sound while projectile is in flight
 */
UCLASS()
class AURA_API AAuraGCN_ProjectileFlight : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AAuraGCN_ProjectileFlight();

	virtual bool OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Audio")
	TObjectPtr<USoundBase> FlightSound;
	
	UPROPERTY()
	float RandomPitchMultiplier;

private:
	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComponent;
};