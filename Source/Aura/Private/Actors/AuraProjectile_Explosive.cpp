// Not Sure Yet

#include "Actors/AuraProjectile_Explosive.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "Tags/AuraTags.h"

AAuraProjectile_Explosive::AAuraProjectile_Explosive()
{
	// Explosive projectiles are configured in blueprint
	// Sphere component is private in base class, configure there
}

void AAuraProjectile_Explosive::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind our custom overlap handler
	if (USphereComponent* SphereComp = FindComponentByClass<USphereComponent>())
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile_Explosive::OnExplosiveOverlap);
	}
}

void AAuraProjectile_Explosive::OnExplosiveOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Don't trigger if we've already exploded or if we hit ourselves/owner
	static bool bHasExploded = false;
	if (bHasExploded || !IsValid(OtherActor) || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	// Only server handles damage
	if (HasAuthority())
	{
		bHasExploded = true;

		// Get explosion location
		const FVector ExplosionLocation = GetActorLocation();

		// Get source ASC from owner
		UAbilitySystemComponent* ASC = nullptr;
		if (AActor* OwnerActor = GetOwner())
		{
			ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
		}

		// Stop flight cue
		if (IsValid(ASC) && FlightCueTag.IsValid())
		{
			ASC->RemoveGameplayCue(FlightCueTag);
		}

		// Play impact cue at explosion center
		if (IsValid(ASC) && ImpactCueTag.IsValid())
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = ExplosionLocation;
			CueParams.Normal = FVector::UpVector;
			CueParams.RawMagnitude = ExplosionRadius; // Pass radius for VFX scaling

			ASC->ExecuteGameplayCue(ImpactCueTag, CueParams);
		}

		// Perform explosion
		ExplodeAtLocation(ExplosionLocation);

		// Destroy projectile
		Destroy();
	}
}

void AAuraProjectile_Explosive::ExplodeAtLocation(const FVector& ExplosionLocation)
{
	if (!HasAuthority() || !DamageEffectSpecHandle.IsValid())
	{
		return;
	}

	// Debug visualization
	if (bDebugDrawExplosion)
	{
		DrawDebugSphere(
			GetWorld(),
			ExplosionLocation,
			ExplosionRadius,
			32,
			FColor::Red,
			false,
			DebugDrawDuration,
			0,
			2.f
		);
	}

	// Find all actors in radius
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	// Use a sphere overlap to find all potential targets
	GetWorld()->OverlapMultiByChannel(
		Overlaps,
		ExplosionLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(ExplosionRadius),
		QueryParams
	);

	// Count valid targets and apply damage
	int32 TargetsHit = 0;

	// Sort by distance (closest first) for MaxTargets limit
	Overlaps.Sort([ExplosionLocation](const FOverlapResult& A, const FOverlapResult& B)
	{
		if (!A.GetActor() || !B.GetActor()) return false;
		const float DistA = FVector::DistSquared(A.GetActor()->GetActorLocation(), ExplosionLocation);
		const float DistB = FVector::DistSquared(B.GetActor()->GetActorLocation(), ExplosionLocation);
		return DistA < DistB;
	});

	UE_LOG(LogTemp, Log, TEXT("Explosion at %s - Found %d potential targets in radius %.0f"), 
		*ExplosionLocation.ToCompactString(), Overlaps.Num(), ExplosionRadius);

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Target = Overlap.GetActor();
		if (!IsValid(Target))
		{
			continue;
		}

		// Check if target has an ability system component
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
		if (!TargetASC)
		{
			continue;
		}

		// Check max targets limit
		if (MaxTargets > 0 && TargetsHit >= MaxTargets)
		{
			UE_LOG(LogTemp, Verbose, TEXT("Reached max targets (%d), stopping explosion damage"), MaxTargets);
			break;
		}

		// Calculate distance from explosion center
		const float Distance = FVector::Dist(ExplosionLocation, Target->GetActorLocation());

		// Apply damage with falloff
		ApplyExplosionDamageToTarget(Target, ExplosionLocation, Distance);

		TargetsHit++;
	}

	UE_LOG(LogTemp, Log, TEXT("Explosion hit %d targets"), TargetsHit);
}

void AAuraProjectile_Explosive::ApplyExplosionDamageToTarget(AActor* Target, const FVector& ExplosionLocation, float DistanceFromCenter)
{
	if (!IsValid(Target) || !DamageEffectSpecHandle.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (!TargetASC)
	{
		return;
	}

	// Calculate damage multiplier based on distance
	float DamageMultiplier = 1.f;
	if (bUseDamageFalloff)
	{
		DamageMultiplier = GetDamageFalloffMultiplier(DistanceFromCenter);
	}

	// Create a copy of the damage spec to modify for this specific target
	FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(*DamageEffectSpecHandle.Data.Get());
	FGameplayEffectSpecHandle ModifiedSpecHandle(MutableSpec);

	// Apply distance-based damage scaling to all SetByCaller magnitudes
	TArray<FGameplayTag> DamageTags;
	MutableSpec->SetByCallerTagMagnitudes.GetKeys(DamageTags);

	for (const FGameplayTag& DamageTag : DamageTags)
	{
		const float OriginalDamage = MutableSpec->GetSetByCallerMagnitude(DamageTag);
		const float ScaledDamage = OriginalDamage * DamageMultiplier;
		MutableSpec->SetSetByCallerMagnitude(DamageTag, ScaledDamage);

		UE_LOG(LogTemp, Verbose, TEXT("Target: %s | Distance: %.0f | Multiplier: %.2f | %s: %.1f -> %.1f"),
			*Target->GetName(), DistanceFromCenter, DamageMultiplier, 
			*DamageTag.ToString(), OriginalDamage, ScaledDamage);
	}

	// Apply the modified damage effect
	TargetASC->ApplyGameplayEffectSpecToSelf(*ModifiedSpecHandle.Data.Get());

	// Trigger HitReact ability
	FGameplayEventData EventData;
	EventData.Instigator = GetOwner();
	EventData.Target = Target;
	TargetASC->HandleGameplayEvent(Aura::Event::HitReact, &EventData);

	// Apply knockback if enabled
	if (bApplyKnockback)
	{
		const FVector KnockbackDirection = (Target->GetActorLocation() - ExplosionLocation).GetSafeNormal();
		const FVector KnockbackVelocity = KnockbackDirection * KnockbackForce;

		// Try to apply impulse if target has a primitive component
		if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(Target->GetRootComponent()))
		{
			if (RootPrimitive->IsSimulatingPhysics())
			{
				RootPrimitive->AddImpulse(KnockbackVelocity, NAME_None, true);
			}
		}

		// Alternative: Apply velocity through Character Movement if available
		if (ACharacter* Character = Cast<ACharacter>(Target))
		{
			if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
			{
				MovementComp->AddImpulse(KnockbackVelocity, true);
			}
		}
	}

	// Debug visualization
	if (bDebugDrawExplosion)
	{
		DrawDebugSphere(
			GetWorld(),
			ExplosionLocation,
			ExplosionRadius,
			16,
			FColor::Orange,
			false,
			DebugDrawDuration,
			0,
			2.f
		);
	}
}

float AAuraProjectile_Explosive::GetDamageFalloffMultiplier(float Distance) const
{
	if (!bUseDamageFalloff || ExplosionRadius <= 0.f)
	{
		return 1.f;
	}

	// Clamp distance to radius
	Distance = FMath::Clamp(Distance, 0.f, ExplosionRadius);

	// Linear falloff from 1.0 at center to MinDamageMultiplier at edge
	// Formula: Multiplier = 1.0 - ((Distance / Radius) * (1.0 - MinMultiplier))
	const float NormalizedDistance = Distance / ExplosionRadius;
	const float Multiplier = 1.f - (NormalizedDistance * (1.f - MinDamageMultiplier));

	return FMath::Clamp(Multiplier, MinDamageMultiplier, 1.f);
}