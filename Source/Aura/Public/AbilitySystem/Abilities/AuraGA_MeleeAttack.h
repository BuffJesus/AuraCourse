#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraGA_MeleeAttack.generated.h"

UCLASS()
class AURA_API UAuraGA_MeleeAttack : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAuraGA_MeleeAttack();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Called by montage event notify to perform the actual attack/damage */
	UFUNCTION(BlueprintCallable, Category = "Aura|MeleeAttack")
	void PerformMeleeAttack();
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	
protected:
	/** Radius for the sphere trace around the weapon socket */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|MeleeAttack")
	float SphereTraceRadius { 50.f };
	
private:
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageCancelled();
	
	UFUNCTION()
	void OnMontageInterrupted();
	
	/** Cache the target for use when montage event fires */
	UPROPERTY()
	TObjectPtr<AActor> CachedTargetActor;
	
	void OnMeleeAttackEvent(const FGameplayEventData* Payload);
	void CleanupEventListener();
};