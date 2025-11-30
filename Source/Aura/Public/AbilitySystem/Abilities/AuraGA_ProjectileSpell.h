// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "Tags/AuraTags.h"
#include "AuraGA_ProjectileSpell.generated.h"

class UAbilityTask_WaitGameplayEvent;
class AAuraProjectile;
class UAbilityTask_PlayMontageAndWait;
class UAuraTargetDataUnderMouse;

UCLASS()
class AURA_API UAuraGA_ProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UAuraGA_ProjectileSpell();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 
	 * Spawns a projectile toward the target location from the specified socket
	 * @param ProjectileTargetLocation - World location to aim the projectile at
	 * @param SocketTag - Gameplay tag identifying which combat socket to spawn from (e.g., CombatSocket.Weapon)
	 * @param bShouldOverridePitch - If true, overrides the calculated pitch with PitchOverrideValue
	 * @param PitchOverrideValue - The pitch angle to use when bShouldOverridePitch is true
	 */
	UFUNCTION(BlueprintCallable, Category = "Aura|Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bShouldOverridePitch = false, float PitchOverrideValue = 0.f);

	/** Ends the ability when the casting montage finishes successfully */
	UFUNCTION()
	void OnMontageCompleted();

	/** Ends the ability when the casting montage is cancelled or interrupted */
	UFUNCTION()
	void OnMontageCancelled();

	/** Called when target data is received (from client or locally) */
	UFUNCTION()
	void OnTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle);

	/** Called when the montage event is received from AnimNotify */
	UFUNCTION()
	void OnMontageEventReceived(FGameplayEventData Payload);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	/** Gameplay Cue for projectile flight sound/effects */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag ProjectileFlightCue;

	/** Gameplay Cue for projectile impact effects */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|GameplayCue", meta = (Categories = "GameplayCue"))
	FGameplayTag ProjectileImpactCue;

	/** Socket tag to use for spawning projectiles (defaults to weapon socket) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Projectile")
	FGameplayTag ProjectileSocketTag { Aura::CombatSocket::Weapon };

	/** Gameplay event tag to wait for from AnimNotify (e.g., Aura.Event.Montage.FireBolt) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Projectile")
	FGameplayTag MontageEventTag { Aura::Event::Montage::FireBolt };

	/** Whether to override the pitch calculation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Projectile")
	bool bOverridePitch = false;

	/** Pitch override value (only used if bOverridePitch is true) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Projectile", meta = (EditCondition = "bOverridePitch"))
	float PitchOverride = 0.f;

private:
        /** Stored montage task to keep it alive for callbacks */
        UPROPERTY()
        TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayMontageTask;

	/** Stored target data task */
	UPROPERTY()
	TObjectPtr<UAuraTargetDataUnderMouse> TargetDataTask;

	/** Stored event wait task */
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> EventWaitTask;

        /** Cached target location from mouse cursor */
        FVector CachedTargetLocation;

        /** Cached target actor resolved from trigger or target data */
        TWeakObjectPtr<AActor> CachedTargetActor;
};