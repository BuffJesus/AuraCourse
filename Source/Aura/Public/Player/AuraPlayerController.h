// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IAuraEnemyInterface;
class UAuraAbilitySystemComponent;
class USplineComponent;

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
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();
	
	void UpdateActorHighlighting(TScriptInterface<IAuraEnemyInterface> NewActor, TScriptInterface<IAuraEnemyInterface> OldActor);
	
	TScriptInterface<IAuraEnemyInterface> LastActor;
	TScriptInterface<IAuraEnemyInterface> ThisActor;
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	void HandleLMBPressed();
	void HandleLMBReleased();
	void HandleLMBHeld();
	void HandleAbilityInput(FGameplayTag InputTag, void(UAuraAbilitySystemComponent::*AbilityFunction)(FGameplayTag));
	
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetASC(); 
	
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
	
	bool FindNavigableDestination(const FVector& ClickLocation, FVector& OutNavLocation) const;
};