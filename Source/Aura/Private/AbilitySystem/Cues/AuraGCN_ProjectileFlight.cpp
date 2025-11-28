// Not Sure Yet

#include "AbilitySystem/Cues/AuraGCN_ProjectileFlight.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraGCN_ProjectileFlight::AAuraGCN_ProjectileFlight()
{
	// Disable ticking for this actor - we only need it for audio management
	PrimaryActorTick.bCanEverTick = false;
	bAutoDestroyOnRemove = true;
}

bool AAuraGCN_ProjectileFlight::OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	Super::OnActive_Implementation(Target, Parameters);

	if (!FlightSound || !IsValid(Target))
	{
		return false;
	}

	// Spawn looping audio at the projectile's location, attached to it
	AudioComponent = UGameplayStatics::SpawnSoundAttached(
		FlightSound,
		Target->GetRootComponent(),
		NAME_None,
		FVector::ZeroVector,
		EAttachLocation::KeepRelativeOffset,
		true, // Stop when attached actor is destroyed
		0.5f, // Volume
		1.0f, // Pitch
		0.0f, // Start time
		nullptr,
		nullptr,
		true // Auto destroy when sound finishes
	);

	return AudioComponent != nullptr;
}

bool AAuraGCN_ProjectileFlight::OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	Super::OnRemove_Implementation(Target, Parameters);

	// Stop and clean up audio component
	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->FadeOut(0.1f, 0.0f); // Quick fade out for smooth stop
	}
	
	Destroy();
	
	return true;
}