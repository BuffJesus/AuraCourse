// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemyCharacter.generated.h"

UCLASS()
class AURA_API AAuraEnemyCharacter : public AAuraBaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemyCharacter();
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Aura|Debug")
	bool bIsHighlighted {false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Debug")
	bool bDrawDebug {true};
};
