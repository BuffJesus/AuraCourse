
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Async/AsyncWork.h"
#include "NavFilters/NavigationQueryFilter.h"
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

// ============================================================================
// ASYNC PATHFINDING TASK
// ============================================================================

class FAsyncPathfindingTask : public FNonAbandonableTask
{
	friend class FAsyncTask<FAsyncPathfindingTask>;

public:
	FAsyncPathfindingTask(
		UWorld* InWorld,
		const FVector& InStartLocation,
		const FVector& InEndLocation,
		const FNavAgentProperties& InNavAgentProps)
		: World(InWorld)
		, StartLocation(InStartLocation)
		, EndLocation(InEndLocation)
		, NavAgentProps(InNavAgentProps)
		, bPathFound(false)
	{
	}

	// Called on worker thread
	void DoWork()
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(AsyncPathfinding);
		
		if (!World.IsValid())
		{
			return;
		}

		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(World.Get());
		if (!NavSystem)
		{
			return;
		}

		// This expensive operation now happens off the game thread
		UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(
			World.Get(),
			StartLocation,
			EndLocation,
			nullptr,
			UNavigationQueryFilter::StaticClass()
		);

		if (Path && Path->IsValid())
		{
			bPathFound = true;
			PathPoints = Path->PathPoints;
		}
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAsyncPathfindingTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	// Results (read on game thread after completion)
	bool IsPathFound() const { return bPathFound; }
	const TArray<FVector>& GetPathPoints() const { return PathPoints; }

private:
	TWeakObjectPtr<UWorld> World;
	FVector StartLocation;
	FVector EndLocation;
	FNavAgentProperties NavAgentProps;
	
	// Results
	bool bPathFound;
	TArray<FVector> PathPoints;
};

/**
 * Player controller handling input, cursor interaction, and click-to-move navigation
 * NOW WITH ASYNC PATHFINDING for smooth performance
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

	// ========================================================================
	// ASYNC PATHFINDING MEMBERS
	// ========================================================================
	
	/** Active async pathfinding task */
	TSharedPtr<FAsyncTask<FAsyncPathfindingTask>> ActivePathfindingTask;
	
	/** Timer for polling pathfinding completion */
	FTimerHandle PathfindingPollTimer;
	
	/** Called when async pathfinding completes */
	void OnPathfindingComplete(const TArray<FVector>& PathPoints);
	
	/** Cancel any active pathfinding request */
	void CancelActivePathfinding();
	
	/** Whether to use async pathfinding (auto-detected based on core count) */
	UPROPERTY(Config, EditAnywhere, Category = "Aura|Performance")
	bool bUseAsyncPathfinding = true;
};