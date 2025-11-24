
// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	const TPair<ECollisionChannel, ECollisionResponse> SphereCollisionResponses[]
	{ {ECC_WorldDynamic, ECR_Overlap}, {ECC_WorldStatic, ECR_Overlap}, {ECC_Pawn, ECR_Overlap} };
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> AudioComponent;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	bool bHit { false };
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	
	UPROPERTY(EditAnywhere, Category = "Aura|Effects")
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	
	UPROPERTY(EditAnywhere, Category = "Aura|Effects")
	TObjectPtr<USoundBase> ImpactSound;
};