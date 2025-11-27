// Not Sure Yet

#include "Player/AuraPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraCollisionChannels.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AuraPlayerAttributeSet.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/AuraEnemyInterface.h"
#include "Tags/AuraTags.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widgets/AuraDamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem { ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()) })
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

	UAuraInputComponent* AuraInputComponent { CastChecked<UAuraInputComponent>(InputComponent) };
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(
		InputConfig, 
		this, 
		&ThisClass::AbilityInputTagPressed, 
		&ThisClass::AbilityInputTagReleased, 
		&ThisClass::AbilityInputTagHeld
	);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector { InputActionValue.Get<FVector2D>() };
	const FRotator Rotation { GetControlRotation() };
	const FRotator YawRotation { 0.f, Rotation.Yaw, 0.f };

	const FVector ForwardDirection { FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) };
	const FVector RightDirection { FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) };

	if (APawn* ControlledPawn { GetPawn<APawn>() })
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	if (LastActor != ThisActor)
	{
		if (LastActor.GetInterface() != nullptr)
		{
			LastActor->UnHighlightActor();
		}
		
		if (ThisActor.GetInterface() != nullptr)
		{
			ThisActor->HighlightActor();
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(Aura::Input::LMB))
	{
		bTargeting = ThisActor.GetInterface() != nullptr;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// Non-LMB inputs go straight to ability system
	if (!InputTag.MatchesTagExact(Aura::Input::LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	
	if (GetASC()) { GetASC()->AbilityInputTagReleased(InputTag); }

	// LMB released - either trigger ability or start auto-run
	if (!bTargeting && !bShiftHeld)
	{
		const APawn* ControlledPawn { GetPawn() };
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// First, trace using the Navigation channel to find a navigable surface
			FHitResult NavChannelHit;
			if (GetHitResultUnderCursor(ECC_Navigation, false, NavChannelHit) && NavChannelHit.bBlockingHit)
			{
				// Project the impact point onto the NavMesh with a custom query extent
				// This ensures we find a valid navmesh point even if the hit is slightly off the mesh
				UNavigationSystemV1* NavSystem { UNavigationSystemV1::GetCurrent(GetWorld()) };
				if (NavSystem)
				{
					FNavLocation ImpactPointNavLocation;
					const FNavAgentProperties& NavAgentProps { GetNavAgentPropertiesRef() };
					const bool bNavLocationFound { NavSystem->ProjectPointToNavigation(
						NavChannelHit.ImpactPoint,
						ImpactPointNavLocation,
						NavigationQueryExtent,
						&NavAgentProps
					) };
					
					if (bNavLocationFound)
					{
						if (UNavigationPath* NavPath { UNavigationSystemV1::FindPathToLocationSynchronously(
							this, ControlledPawn->GetActorLocation(), ImpactPointNavLocation.Location) })
						{
							Spline->ClearSplinePoints();
							for (const FVector& PointLoc : NavPath->PathPoints)
							{
								Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
							}
							CachedDestination = NavPath->PathPoints.Last();
							bAutoRunning = true;
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
	// Non-LMB inputs go straight to ability system
	if (!InputTag.MatchesTagExact(Aura::Input::LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	// LMB held - either trigger ability or move toward cursor
	if (bTargeting || bShiftHeld)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

		if (APawn* ControlledPawn { GetPawn() })
		{
			const FVector WorldDirection { (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal() };
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (!AuraAbilitySystemComponent)
	{
		// Cast once and cache - happens only once per controller
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>())
		);
	}
	return AuraAbilitySystemComponent;
}

AAuraHUD* AAuraPlayerController::GetAuraHUD()
{
	if (!AuraHUD)
	{
		// Cast once and cache - happens only once per controller
		AuraHUD = Cast<AAuraHUD>(GetHUD());
	}
	return AuraHUD;
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	
	if (APawn* ControlledPawn { GetPawn() })
	{
		const FVector LocationOnSpline { Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World) };
		const FVector Direction { Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World) };
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}