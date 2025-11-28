// Not Sure Yet

#include "Actors/AuraProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraCollisionChannels.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Tags/AuraTags.h"

namespace 
{
	constexpr TPair<ECollisionChannel, ECollisionResponse> SphereCollisionResponses[]
	{ {ECC_WorldDynamic, ECR_Overlap}, {ECC_WorldStatic, ECR_Overlap}, {ECC_Pawn, ECR_Overlap} };
    
	constexpr float Speed = 550.f;
	constexpr float GravityScale = 0.f;
	constexpr float LifeSpan = 10.f;
}

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	for (const auto& [Channel, Response] : SphereCollisionResponses)
	{ Sphere->SetCollisionResponseToChannel(Channel, Response); }

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = GravityScale;
}

void AAuraProjectile::BeginPlay()
{
Super::BeginPlay();

SetLifeSpan(LifeSpan);
Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

// Cache the source ASC from the owner
if (AActor* OwnerActor = GetOwner())
{
SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
Sphere->IgnoreActorWhenMoving(OwnerActor, true);
}
 
 	// Start flight cue if configured (Add = start looping)
 	if (SourceASC && FlightCueTag.IsValid())
 	{
 		FGameplayCueParameters CueParams;
 		CueParams.Location = GetActorLocation();
 		// Target is the projectile so the GC actor can attach to it
 		
 		SourceASC->AddGameplayCue(FlightCueTag, CueParams);
 	}
 }

void AAuraProjectile::Destroyed()
{
	// Stop flight cue if we're being destroyed without hitting (e.g., timed out)
	if (!bHit && IsValid(SourceASC) && FlightCueTag.IsValid())
	{
		SourceASC->RemoveGameplayCue(FlightCueTag);
		
		// Also play impact effects on clients when projectile is destroyed
		if (!HasAuthority() && ImpactCueTag.IsValid())
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetActorLocation();
			
			SourceASC->ExecuteGameplayCue(ImpactCueTag, CueParams);
		}
	}

	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (HasAuthority())
	{
		bHit = true;

		// Apply damage
		if (DamageEffectSpecHandle.IsValid())
		{
			if (UAbilitySystemComponent* TargetASC { UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor) })
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());

				// Trigger HitReact ability
				FGameplayEventData EventData;
				EventData.Instigator = GetOwner();
				EventData.Target = OtherActor;
				TargetASC->HandleGameplayEvent(Aura::Event::HitReact, &EventData);
			}
			else
			{
				UE_LOG(LogTemp, Verbose, TEXT("AuraProjectile hit %s but found no ASC to apply damage"), *OtherActor->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AuraProjectile %s has no valid damage spec; destroying without applying effects"), *GetName());
		}

		// Stop flight cue (Remove = stop looping)
		if (IsValid(SourceASC) && FlightCueTag.IsValid())
			SourceASC->RemoveGameplayCue(FlightCueTag);

		// Play impact cue
		if (IsValid(SourceASC) && ImpactCueTag.IsValid())
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetActorLocation();
			CueParams.Normal = SweepResult.ImpactNormal;

			SourceASC->ExecuteGameplayCue(ImpactCueTag, CueParams);
		}

		Destroy();
	}
	else
	{
		bHit = true;
	}
}
