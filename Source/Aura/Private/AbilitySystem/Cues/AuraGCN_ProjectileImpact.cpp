// Not Sure Yet

#include "AbilitySystem/Cues/AuraGCN_ProjectileImpact.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

bool UAuraGCN_ProjectileImpact::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	// Use the location from Parameters for accurate impact positioning
	const FVector ImpactLocation = Parameters.Location.IsZero() ? 
		(IsValid(Target) ? Target->GetActorLocation() : FVector::ZeroVector) : 
		Parameters.Location;

	if (ImpactLocation.IsZero())
	{
		return false;
	}

	// Get world context for spawning effects
	UWorld* World = IsValid(Target) ? Target->GetWorld() : nullptr;
	if (!World)
	{
		return false;
	}

	// Play impact sound
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			World,
			ImpactSound,
			ImpactLocation,
			FRotator::ZeroRotator
		);
	}

	// Spawn impact particle effect
	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			ImpactEffect,
			ImpactLocation,
			Parameters.Normal.Rotation() // Use surface normal if available
		);
	}

	return true;
}