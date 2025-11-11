// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Aura/Aura.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AbilityRefs.AttributeSet;}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|AbilitySystem")
	FAuraAbilityReferences AbilityRefs;
};
