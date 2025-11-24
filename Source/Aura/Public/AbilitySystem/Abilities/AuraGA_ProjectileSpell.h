
// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraGA_ProjectileSpell.generated.h"

class AAuraProjectile;
/**
 * 
 */
UCLASS()
class AURA_API UAuraGA_ProjectileSpell : public UAuraGameplayAbility
{
	GENERATED_BODY()

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Aura|Projectile")
	void SpawnProjectile(const FVector& TargetLocation);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	/** If true, spawn cosmetic projectile on client for instant feedback */
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Projectile")
	bool bUseCosmeticPrediction = false;
};