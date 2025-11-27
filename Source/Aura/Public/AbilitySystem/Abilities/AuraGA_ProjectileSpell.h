// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "AuraGA_ProjectileSpell.generated.h"

class AAuraProjectile;

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** Gameplay Cue for projectile flight sound/effects */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag ProjectileFlightCue;

	/** Gameplay Cue for projectile impact effects */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag ProjectileImpactCue;
};