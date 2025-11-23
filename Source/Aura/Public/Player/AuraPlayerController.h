
// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

struct FGameplayTag;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IAuraEnemyInterface;
class UAuraAbilitySystemComponent;
class USplineComponent;

/**
 * 
 */
UCLASS(Abstract)
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	// Input
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	void Move(const FInputActionValue& InputActionValue);
	
	// Cursor Interaction
	void CursorTrace();
	void UpdateActorHighlighting(TScriptInterface<IAuraEnemyInterface> NewActor, TScriptInterface<IAuraEnemyInterface> OldActor);
	
	TScriptInterface<IAuraEnemyInterface> LastActor;
	TScriptInterface<IAuraEnemyInterface> ThisActor;
	
	// Ability Input
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	void HandleLMBPressed();
	void HandleLMBReleased();
	void HandleLMBHeld();
	void HandleAbilityInput(FGameplayTag InputTag, bool bIsTargeting, void(UAuraAbilitySystemComponent::*AbilityFunction)(FGameplayTag));
	
	// Ability System
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetASC(); 
	
	// Movement
	FVector CachedDestination { FVector::ZeroVector };
	float FollowTime { 0.0f };
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Movement")
	float ShortPressThreshold { 0.5f };
	
	bool bAutoRunning { false };
	bool bTargeting { false };
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Movement")
	float AutoRunAcceptanceRadius { 50.0f };
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|Movement")
	FVector NavigationQueryExtent { 400.0f, 400.0f, 250.0f };
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	
	void AutoRun();
	void StartAutoRunToLocation(const FVector& Destination);
	bool FindNavigableDestination(const FVector& ClickLocation, FVector& OutNavLocation);
};