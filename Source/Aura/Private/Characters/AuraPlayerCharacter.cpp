// Not Sure Yet

#include "Characters/AuraPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"

AAuraPlayerCharacter::AAuraPlayerCharacter()
{
	// Configure character movement for top-down gameplay
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, RotationRate, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AAuraPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitializeAbilityActorInfo();
	
	// Defer to next frame to ensure ASC is fully initialized
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			// Server only: Initialize default attributes (these will replicate to clients)
			InitializeDefaultAttributes();
			
			// Init abilities on server
			AddCharacterAbilities();
		});
	}
}

void AAuraPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitializeAbilityActorInfo();
}

int32 AAuraPlayerCharacter::GetPlayerLevel() const
{
	const AAuraPlayerState* AuraPlayerState { GetPlayerState<AAuraPlayerState>() };
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraPlayerCharacter::InitializeAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState { GetPlayerState<AAuraPlayerState>() };
	check(AuraPlayerState);
	
	// Use typed getter - no cast needed!
	AbilitySystemComponent = AuraPlayerState->GetAuraAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	AbilitySystemComponent->AbilityActorInfoSet();
	
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD { Cast<AAuraHUD>(AuraPlayerController->GetHUD()) })
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}