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
	if (AActor* Owner = GetOwner())
	{
		SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
	}

	// Start flight sound cue (Add = start looping)
	if (SourceASC)
	{
		FGameplayCueParameters CueParams;
		CueParams.SourceObject = this;
		CueParams.Location = GetActorLocation();
		
		SourceASC->AddGameplayCue(Aura::GameplayCue::Aura::Projectile::Flight, CueParams);
	}
}

void AAuraProjectile::Destroyed()
{
	// Stop flight sound if we're being destroyed without hitting
	// (e.g., timed out)
	if (!bHit && SourceASC)
	{
		SourceASC->RemoveGameplayCue(Aura::GameplayCue::Aura::Projectile::Flight);
		
		// Also play impact effects on clients when projectile is destroyed
		if (!HasAuthority())
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = GetActorLocation();
			
			SourceASC->ExecuteGameplayCue(Aura::GameplayCue::Aura::Projectile::Impact, CueParams);
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

		// Stop flight sound (Remove = stop looping)
		if (SourceASC)
		{
			SourceASC->RemoveGameplayCue(Aura::GameplayCue::Aura::Projectile::Flight);
			
			// Play impact effects
			FGameplayCueParameters CueParams;
			CueParams.Location = GetActorLocation();
			CueParams.Normal = SweepResult.ImpactNormal;
			
			SourceASC->ExecuteGameplayCue(Aura::GameplayCue::Aura::Projectile::Impact, CueParams);
		}
		
		Destroy();
	}
	else { bHit = true; }
}