// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IAuraEnemyInterface;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

/**
 * Player controller handling input, cursor interaction, and click-to-move navigation
 */
UCLASS()
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
	//~=============================================================================
	// Input
	//~=============================================================================
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	void Move(const FInputActionValue& InputActionValue);

	//~=============================================================================
	// Cursor Trace & Highlighting
	//~=============================================================================
	
	void CursorTrace();
	
	/** Cached hit result from cursor trace */
	FHitResult CursorHit;
	
	/** Previously hovered enemy actor */
	TScriptInterface<IAuraEnemyInterface> LastActor;
	
	/** Currently hovered enemy actor */
	TScriptInterface<IAuraEnemyInterface> ThisActor;

	//~=============================================================================
	// Ability Input
	//~=============================================================================
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	//~=============================================================================
	// Click-to-Move & Auto-Run
	//~=============================================================================
	
	FVector CachedDestination { FVector::ZeroVector };
	float FollowTime { 0.f };
	float ShortPressThreshold { 0.5f };
	bool bAutoRunning { false };
	bool bTargeting { false };

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius { 50.f };

	/** Extents for navigation query when projecting to NavMesh (default is 50,50,250) */
	UPROPERTY(EditDefaultsOnly)
	FVector NavigationQueryExtent { 400.0f, 400.0f, 250.0f };

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();
};