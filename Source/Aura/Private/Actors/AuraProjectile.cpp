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
	}

	// Start flight cue if configured (Add = start looping)
	if (SourceASC && FlightCueTag.IsValid())
	{
		FGameplayCueParameters CueParams;
		CueParams.SourceObject = this;
		CueParams.Location = GetActorLocation();
		
		SourceASC->AddGameplayCue(FlightCueTag, CueParams);
	}
}

void AAuraProjectile::Destroyed()
{
	// Stop flight cue if we're being destroyed without hitting (e.g., timed out)
	if (!bHit && SourceASC && FlightCueTag.IsValid())
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
	if (HasAuthority())
	{
		// Apply damage
		if (UAbilitySystemComponent* TargetASC { UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor) })
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			
			// Trigger HitReact ability
			FGameplayEventData EventData;
			EventData.Instigator = GetOwner();
			EventData.Target = OtherActor;
			TargetASC->HandleGameplayEvent(Aura::Event::HitReact, &EventData);
		}

		// Stop flight cue (Remove = stop looping)
		if (SourceASC && FlightCueTag.IsValid())
		{
			SourceASC->RemoveGameplayCue(FlightCueTag);
		}
		
		// Play impact cue
		if (SourceASC && ImpactCueTag.IsValid())
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