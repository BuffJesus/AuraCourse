#pragma once

#include "CoreMinimal.h"
#include "AuraBaseCharacter.h"
#include "AbilitySystem/Data/AuraCharacterClassInfo.h"
#include "UI/Controllers/AuraOverlayWidgetController.h"
#include "Interaction/AuraEnemyInterface.h"
#include "AuraEnemyCharacter.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

UCLASS()
class AURA_API AAuraEnemyCharacter : public AAuraBaseCharacter, public IAuraEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemyCharacter();
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	virtual int32 GetCharacterLevel() const override;
	
	virtual void Die() override;
	virtual void MulticastHandleDeath_Implementation() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UPROPERTY(BlueprintReadOnly, Category = "Aura|Combat")
	bool bHitReacting { false };
	
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	float BaseWalkSpeed { 250.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Combat")
	float LifeSpan { 5.f };

protected:
	virtual void BeginPlay() override;
	virtual void InitializeAbilityActorInfo() override;
	

        virtual UAnimMontage* GetAttackMontage_Implementation() const override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aura|UI")
	TObjectPtr<UWidgetComponent> HealthBar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|UI")
	FVector2D HealthBarDrawSize { FVector2D(500.f, 50.f) };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|CharacterClassDefaults")
	int32 Level { 1 };
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|CharacterClassDefaults")
	ECharacterClass CharacterClass { ECharacterClass::Warrior };
	
	virtual ECharacterClass GetCharacterClass() const override { return CharacterClass; }
	
	UPROPERTY(EditAnywhere, Category = "Aura|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
	
};