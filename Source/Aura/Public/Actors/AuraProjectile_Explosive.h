// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "Actors/AuraProjectile.h"
#include "AuraProjectile_Explosive.generated.h"

/**
 * Explosive projectile that deals AOE damage on impact
 * Perfect for testing parallel damage calculation with multiple targets
 */
UCLASS()
class AURA_API AAuraProjectile_Explosive : public AAuraProjectile
{
	GENERATED_BODY()

public:
	AAuraProjectile_Explosive();

protected:
	/** Handle explosion on impact - calls parent then performs AOE damage */
	virtual void BeginPlay() override;
	
	/** Custom overlap handler that triggers explosion */
	UFUNCTION()
	void OnExplosiveOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Radius for AOE damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Explosion", meta = (ClampMin = "50.0", ClampMax = "1000.0"))
	float ExplosionRadius = 300.f;

	/** Damage falloff curve - how much damage decreases with distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Explosion")
	bool bUseDamageFalloff = true;

	/** Minimum damage multiplier at max range (0.0 = no damage, 1.0 = full damage) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Explosion", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bUseDamageFalloff"))
	float MinDamageMultiplier = 0.5f;

	/** Whether to apply knockback to hit enemies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Explosion")
	bool bApplyKnockback = false;

	/** Knockback force strength */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Explosion", meta = (EditCondition = "bApplyKnockback"))
	float KnockbackForce = 500.f;

	/** Maximum number of targets to hit (0 = unlimited) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Explosion", meta = (ClampMin = "0", ClampMax = "50"))
	int32 MaxTargets = 0;

	/** Debug: Draw explosion radius */
	UPROPERTY(EditAnywhere, Category = "Aura|Debug")
	bool bDebugDrawExplosion = false;

	/** Debug: Duration to show debug sphere */
	UPROPERTY(EditAnywhere, Category = "Aura|Debug", meta = (EditCondition = "bDebugDrawExplosion"))
	float DebugDrawDuration = 2.f;

private:
	/** Perform explosion damage at location */
	UFUNCTION()
	void ExplodeAtLocation(const FVector& ExplosionLocation);

	/** Apply damage to a single target with falloff */
	void ApplyExplosionDamageToTarget(AActor* Target, const FVector& ExplosionLocation, float DistanceFromCenter);

	/** Calculate damage multiplier based on distance */
	float GetDamageFalloffMultiplier(float Distance) const;
};