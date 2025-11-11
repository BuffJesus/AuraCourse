// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemyCharacter.generated.h"

class UAuraAbilitySystemComponent;
class UAuraAttributeSet;

UCLASS()
class AURA_API AAuraEnemyCharacter : public AAuraBaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemyCharacter();
	
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	// Typed accessor
	UAuraAbilitySystemComponent* GetAuraASC() const;

protected:
	virtual void BeginPlay() override;
};
