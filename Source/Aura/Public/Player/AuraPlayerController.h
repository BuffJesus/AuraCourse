// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UAuraDamageTextComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IAuraEnemyInterface;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;
class AAuraHUD;

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
	
	/** Get the current cursor hit result */
	FORCEINLINE FHitResult GetCursorHit() const { return CursorHit; }
	
	/** Get cached AuraHUD - casts and caches on first call */
	AAuraHUD* GetAuraHUD();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	/** Cached hit result from cursor trace */
	FHitResult CursorHit;

private:
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category = "Aura|Input")
	TObjectPtr<UInputAction> ShiftAction;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();
	
	/** Previously hovered enemy actor */
	TScriptInterface<IAuraEnemyInterface> LastActor;
	
	/** Currently hovered enemy actor */
	TScriptInterface<IAuraEnemyInterface> ThisActor;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	void ShiftPressed() { bShiftHeld = true; };
	void ShiftReleased() { bShiftHeld = false; };
	bool bShiftHeld { false };

	/** Cached ASC - initialized on first access */
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	/** Cached HUD - initialized on first access */
	UPROPERTY()
	TObjectPtr<AAuraHUD> AuraHUD;

	UAuraAbilitySystemComponent* GetASC();

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
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|DamageText")
	TSubclassOf<UAuraDamageTextComponent> DamageTextComponentClass;
};