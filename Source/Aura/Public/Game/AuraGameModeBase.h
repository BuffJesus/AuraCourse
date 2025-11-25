// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UAuraCharacterClassInfo;

/**
 * 
 */
UCLASS(Abstract)
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|CharacterClassDefaults")
	TObjectPtr<UAuraCharacterClassInfo> CharacterClassInfo;
};
