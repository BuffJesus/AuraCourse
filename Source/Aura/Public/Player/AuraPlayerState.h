// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Data/AuraCharacterClassInfo.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAuraPlayerAttributeSet;
class UAbilitySystemComponent;
class UAuraAbilitySystemComponent;

UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Interface requirement - returns base type
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// Typed getter - returns UAuraAbilitySystemComponent (no cast needed!)
	FORCEINLINE UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	// Typed getter - returns UAuraPlayerAttributeSet (no cast needed!)
	FORCEINLINE UAuraPlayerAttributeSet* GetAuraAttributeSet() const { return AttributeSet; }
	
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }

protected:
	// Store as typed pointer - no casting needed when accessing!
	UPROPERTY(VisibleAnywhere, Category = "Aura|GAS")
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;

	// Store as typed pointer for player-specific attributes - no casting needed when accessing!
	UPROPERTY()
	TObjectPtr<UAuraPlayerAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Level, Category = "Aura|CharacterClassDefaults")
	int32 Level { 1 };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|CharacterClassDefaults")
	ECharacterClass DefaultClass { ECharacterClass::Elementalist };

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
};