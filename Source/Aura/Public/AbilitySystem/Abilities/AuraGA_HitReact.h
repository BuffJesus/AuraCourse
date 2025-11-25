
// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraGA_HitReact.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGA_HitReact : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAuraGA_HitReact();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|HitReact")
	TSubclassOf<UGameplayEffect> HitReactEffect;

private:
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageCancelled();
	
	UFUNCTION()
	void OnMontageInterrupted();
};