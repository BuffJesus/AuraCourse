// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "AuraGCN_ProjectileImpact.generated.h"

class UNiagaraSystem;

/**
 * Execute-based Gameplay Cue for projectile impact effects
 * Plays one-shot sound and particle effect
 */
UCLASS()
class AURA_API UAuraGCN_ProjectileImpact : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Effects")
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Audio")
	TObjectPtr<USoundBase> ImpactSound;
};