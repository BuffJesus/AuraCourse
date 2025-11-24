// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class UAuraAttributeSet;
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
	
	// Typed getter - returns UAuraAttributeSet (no cast needed!)
	FORCEINLINE UAuraAttributeSet* GetAuraAttributeSet() const { return AttributeSet; }
	
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }

protected:
	// Store as typed pointer - no casting needed when accessing!
	UPROPERTY(VisibleAnywhere, Category = "Aura|GAS")
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;

	// Store as typed pointer - no casting needed when accessing!
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level, Category = "Aura")
	int32 Level { 1 };

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
};