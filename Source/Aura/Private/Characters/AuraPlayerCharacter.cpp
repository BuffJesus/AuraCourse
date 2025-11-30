
#include "Characters/AuraPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AuraAttributeSet.h"
#include "AbilitySystem/Attributes/AuraPlayerAttributeSet.h"
#include "Player/AuraPlayerController.h"
#include "Tags/AuraTags.h"
#include "UI/HUD/AuraHUD.h"

AAuraPlayerCharacter::AAuraPlayerCharacter()
{
	CharacterTags.AddTag(Aura::Entities::Player);
	
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
	
	// FIXED: Use weak pointer in lambda to prevent accessing destroyed actor
	if (GetWorld())
	{
		TWeakObjectPtr<AAuraPlayerCharacter> WeakThis(this);
		GetWorld()->GetTimerManager().SetTimerForNextTick([WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				// Server only: Initialize default attributes (these will replicate to clients)
				WeakThis->InitializeDefaultAttributes();
				
				// CRITICAL: Grant abilities on server
				// With Mixed replication mode, abilities don't auto-replicate, 
				// so we must grant them on both server and client
				WeakThis->AddCharacterAbilities();
			}
		});
	}
}

void AAuraPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitializeAbilityActorInfo();
	
	// CRITICAL FIX: Grant abilities on client as well!
	// The ASC uses Mixed replication mode which replicates effects but NOT abilities
	// Abilities must be explicitly granted on both server and client
	// This is why the client couldn't activate the ability - it didn't have it!
	if (GetWorld())
	{
		TWeakObjectPtr<AAuraPlayerCharacter> WeakThis(this);
		GetWorld()->GetTimerManager().SetTimerForNextTick([WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->AddCharacterAbilities();
			}
		});
	}
}

int32 AAuraPlayerCharacter::GetCharacterLevel() const
{
	const AAuraPlayerState* AuraPlayerState { GetPlayerState<AAuraPlayerState>() };
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

ECharacterClass AAuraPlayerCharacter::GetCharacterClass() const
{
	const AAuraPlayerState* AuraPlayerState { GetPlayerState<AAuraPlayerState>() };
	check(AuraPlayerState);
	return AuraPlayerState->GetCharacterClass();
}

void AAuraPlayerCharacter::InitializeAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState { GetPlayerState<AAuraPlayerState>() };
	check(AuraPlayerState);
	
	AbilitySystemComponent = AuraPlayerState->GetAuraAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);
	AbilitySystemComponent->AbilityActorInfoSet();
	
	// GetAuraAttributeSet() returns UAuraPlayerAttributeSet*, cast to base UAuraAttributeSet*
	// The typed getter avoids casting internally, but we still need to cast to the base type here
	AttributeSet = Cast<UAuraAttributeSet>(AuraPlayerState->GetAuraAttributeSet());

	// Cache controller on first access - cast happens once
	if (!AuraPlayerController) { AuraPlayerController = GetController<AAuraPlayerController>(); }

	if (AuraPlayerController)
	{
		// Use cached controller's GetAuraHUD() - no cast needed!
		if (AAuraHUD* AuraHUD { AuraPlayerController->GetAuraHUD() })
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}