// Not Sure Yet


#include "Actors/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraCollisionChannels.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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
	
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");

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
	
	// Only authoritative projectiles handle overlaps
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);
}

void AAuraProjectile::PlayImpactEffects() const
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
}

void AAuraProjectile::Destroyed()
{
	if (!bHit && !HasAuthority()) { PlayImpactEffects(); }
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayImpactEffects();
	
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC { UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor) })
		{ TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get()); }
		
		Destroy();
	}
	else { bHit = true; }
}

