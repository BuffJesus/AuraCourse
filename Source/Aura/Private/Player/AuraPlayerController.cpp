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

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
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

	if (LastActor != ThisActor)
	{
		UpdateActorHighlighting(ThisActor, LastActor);
	}
}

void AAuraPlayerController::UpdateActorHighlighting(TScriptInterface<IAuraEnemyInterface> NewActor, TScriptInterface<IAuraEnemyInterface> OldActor)
{
	if (OldActor.GetInterface() != nullptr)
	{
		OldActor->UnHighlightActor();
	}
	
	if (NewActor.GetInterface() != nullptr)
	{
		NewActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(Aura::Input::LMB))
	{
		HandleLMBPressed();
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(Aura::Input::LMB))
	{
		HandleLMBReleased();
	}
	else
	{
		HandleAbilityInput(InputTag, false, &UAuraAbilitySystemComponent::AbilityInputTagReleased);
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(Aura::Input::LMB))
	{
		HandleLMBHeld();
	}
	else
	{
		HandleAbilityInput(InputTag, false, &UAuraAbilitySystemComponent::AbilityInputTagHeld);
	}
}

void AAuraPlayerController::HandleLMBPressed()
{
	bTargeting = ThisActor != nullptr;
	bAutoRunning = false;
}

void AAuraPlayerController::HandleLMBReleased()
{
	if (bTargeting)
	{
		HandleAbilityInput(Aura::Input::LMB, true, &UAuraAbilitySystemComponent::AbilityInputTagReleased);
	}
	else if (FollowTime <= ShortPressThreshold)
	{
		FHitResult NavChannelHit;
		if (GetHitResultUnderCursor(ECC_Navigation, false, NavChannelHit) && NavChannelHit.bBlockingHit)
		{
			FVector NavLocation;
			if (FindNavigableDestination(NavChannelHit.ImpactPoint, NavLocation))
			{
				StartAutoRunToLocation(NavLocation);
			}
		}
	}
	
	FollowTime = 0.f;
	bTargeting = false;
}

void AAuraPlayerController::HandleLMBHeld()
{
	if (bTargeting)
	{
		HandleAbilityInput(Aura::Input::LMB, true, &UAuraAbilitySystemComponent::AbilityInputTagHeld);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		
		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Navigation, false, Hit))
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

void AAuraPlayerController::HandleAbilityInput(FGameplayTag InputTag, bool bIsTargeting, void(UAuraAbilitySystemComponent::*AbilityFunction)(FGameplayTag))
{
	if (UAuraAbilitySystemComponent* ASC = GetASC())
	{
		(ASC->*AbilityFunction)(InputTag);
	}
}

bool AAuraPlayerController::FindNavigableDestination(const FVector& ClickLocation, FVector& OutNavLocation)
{
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return false;

	FNavLocation NavLocation;
	const FNavAgentProperties& NavAgentProps = GetNavAgentPropertiesRef();
	
	if (NavSystem->ProjectPointToNavigation(ClickLocation, NavLocation, NavigationQueryExtent, &NavAgentProps))
	{
		OutNavLocation = NavLocation.Location;
		return true;
	}
	
	return false;
}

void AAuraPlayerController::StartAutoRunToLocation(const FVector& Destination)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
		this, 
		ControlledPawn->GetActorLocation(), 
		Destination
	);
	
	if (!NavPath || NavPath->PathPoints.Num() == 0) return;

	Spline->ClearSplinePoints();
	for (const FVector& PointLoc : NavPath->PathPoints)
	{
		Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
		DrawDebugSphere(GetWorld(), PointLoc, 10.f, 10, FColor::Green, false, 5.f);
	}
	
	CachedDestination = NavPath->PathPoints.Last();
	bAutoRunning = true;
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
		ControlledPawn->GetActorLocation(), 
		ESplineCoordinateSpace::World
	);
	const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
		LocationOnSpline, 
		ESplineCoordinateSpace::World
	);
	ControlledPawn->AddMovementInput(Direction);

	const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
	if (DistanceToDestination < AutoRunAcceptanceRadius)
	{
		bAutoRunning = false;
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>())
		);
	}
	return AuraAbilitySystemComponent;
}