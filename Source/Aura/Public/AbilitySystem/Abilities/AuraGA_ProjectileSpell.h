// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "AuraGA_ProjectileSpell.generated.h"

class AAuraProjectile;
class UAbilityTask_PlayMontageAndWait;
class UAnimInstance;

UCLASS()
class AURA_API UAuraGA_ProjectileSpell : public UAuraDamageGameplayAbility
{
        GENERATED_BODY()

public:
UAuraGA_ProjectileSpell();

protected:
virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

UFUNCTION(BlueprintCallable, Category = "Aura|Projectile")
void SpawnProjectile(const FVector& TargetLocation);

/** Ends the ability when the casting montage finishes successfully */
UFUNCTION()
void OnMontageCompleted();

/** Ends the ability when the casting montage is cancelled or interrupted */
UFUNCTION()
void OnMontageCancelled();

/** End delegate used when we have to fall back to playing the montage directly */
UFUNCTION()
void OnFallbackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

/** Clears the fallback montage delegate to avoid duplicate callbacks */
void ClearFallbackMontageDelegate();

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Projectile")
TSubclassOf<AAuraProjectile> ProjectileClass;

/** Gameplay Cue for projectile flight sound/effects */
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|GameplayCue", meta = (Categories = "GameplayCue"))
FGameplayTag ProjectileFlightCue;

/** Gameplay Cue for projectile impact effects */
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|GameplayCue", meta = (Categories = "GameplayCue"))
FGameplayTag ProjectileImpactCue;

/** Stored montage task to keep it alive for callbacks */
UPROPERTY()
TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayMontageTask;

/** Backup anim instance & delegate in case ability task creation fails */
TWeakObjectPtr<UAnimInstance> FallbackAnimInstance;

FOnMontageEnded FallbackMontageDelegate;
};