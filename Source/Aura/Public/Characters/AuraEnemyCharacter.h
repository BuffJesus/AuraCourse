// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "AbilitySystem/Data/AuraCharacterClassInfo.h"
#include "UI/Controllers/AuraOverlayWidgetController.h"
#include "Interaction/AuraEnemyInterface.h"
#include "AuraEnemyCharacter.generated.h"

class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemyCharacter : public AAuraBaseCharacter, public IAuraEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemyCharacter();
	
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	virtual int32 GetPlayerLevel() const override;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aura|UI")
	TObjectPtr<UWidgetComponent> HealthBar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|UI")
	FVector2D HealthBarDrawSize { FVector2D(500.f, 50.f) };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|CharacterClassDefaults")
	int32 Level { 1 };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|CharacterClassDefaults")
	ECharacterClass DefaultClass { ECharacterClass::Warrior };
};