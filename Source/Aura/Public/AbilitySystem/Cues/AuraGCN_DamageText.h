// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "AuraGCN_DamageText.generated.h"

class UAuraDamageTextComponent;

/**
 * Gameplay Cue for displaying floating damage text
 */
UCLASS()
class AURA_API UAuraGCN_DamageText : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Aura|DamageText")
	TSubclassOf<UAuraDamageTextComponent> DamageTextComponentClass;
};