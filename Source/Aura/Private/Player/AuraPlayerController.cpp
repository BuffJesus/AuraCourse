// Not Sure Yet


#include "Player/AuraPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraCollisionChannels.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/AuraEnemyInterface.h"
#include "Tags/AuraTags.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) { return; }
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		if (const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length(); 
			DistanceToDestination < AutoRunAcceptanceRadius) { bAutoRunning = false; }
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("SetupInputComponent Called"));

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(FVector::ForwardVector, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(FVector::RightVector, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	// Check if actors changed AND are different
	if (LastActor != ThisActor)
	{
		// Only call if the interface is actually valid
		if (LastActor.GetInterface() != nullptr) { LastActor->UnHighlightActor(); }
		if (ThisActor.GetInterface() != nullptr) { ThisActor->HighlightActor(); }
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(Aura::Input::LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(Aura::Input::LMB)) { if (GetASC()) { GetASC()->AbilityInputTagReleased(InputTag); } return;}
	if (bTargeting) { if (GetASC()) { GetASC()->AbilityInputTagReleased(InputTag); } }
	else
	{
		if (APawn* ControlledPawn = GetPawn(); FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// Use custom Navigation trace channel to get impact point
			FHitResult NavChannelHit;
			GetHitResultUnderCursor(ECC_Navigation, false, NavChannelHit);
			
			if (NavChannelHit.bBlockingHit)
			{
				// Project the impact point onto the NavMesh with a larger query extent
				// This ensures we find a valid NavMesh point even when clicking on or near obstacles
				if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
				{
					FNavLocation ImpactPointNavLocation;
					// NOTE: Default Query Extent = FVector(50.0f, 50.0f, 250.0f)
					// Using a much larger extent to handle clicks on/near obstacles
					const FVector QueryingExtent{400.0f, 400.0f, 250.0f};
					const FNavAgentProperties& NavAgentProps = GetNavAgentPropertiesRef();
					
					const bool bNavLocationFound = NavSystem->ProjectPointToNavigation(
						NavChannelHit.ImpactPoint, 
						ImpactPointNavLocation, 
						QueryingExtent, 
						&NavAgentProps
					);
					
					if (bNavLocationFound)
					{
						if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
							this, 
							ControlledPawn->GetActorLocation(), 
							ImpactPointNavLocation.Location))
						{
							if (NavPath->PathPoints.Num() > 0)
							{
								Spline->ClearSplinePoints();
								for (const FVector& PointLoc : NavPath->PathPoints)
								{
									Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
									DrawDebugSphere(GetWorld(), PointLoc, 10.f, 10, FColor::Green, false, 5.f);
								}
								CachedDestination = NavPath->PathPoints.Last();
								bAutoRunning = true;
							}
						}
					}
				}
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(Aura::Input::LMB)) { if (GetASC()) { GetASC()->AbilityInputTagHeld(InputTag); } return;}
	if (bTargeting) { if (GetASC()) { GetASC()->AbilityInputTagHeld(InputTag); } }
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		
		// Use Navigation channel for cursor destination
		if (FHitResult Hit; GetHitResultUnderCursor(ECC_Navigation, false, Hit)) 
		{ 
			CachedDestination = Hit.ImpactPoint; 
		}
		
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>
		(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}