// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "Interaction/AuraCombatInterface.h"
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
	void SpawnProjectile();
	IAuraCombatInterface* GetCombatInterfaceFromAvatar() const;
	FTransform GetProjectileSpawnTransform(IAuraCombatInterface* CombatInterface) const;
	AAuraProjectile* CreateProjectile(const FTransform& SpawnTransform);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;
};
