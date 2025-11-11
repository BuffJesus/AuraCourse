// Not Sure Yet

#include "Characters/AuraPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

DEFINE_LOG_CATEGORY_STATIC(LogAuraPlayer, Log, All);

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
	InitAbilityActorInfo();
}

void AAuraPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

void AAuraPlayerCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	if (!AuraPS)
	{
		UE_LOG(LogAuraPlayer, Error, TEXT("Failed to get AuraPlayerState for %s"), *GetName());
		return;
	}

	UAuraAbilitySystemComponent* AuraASC = AuraPS->GetAuraASC();
	if (!AuraASC)
	{
		UE_LOG(LogAuraPlayer, Error, TEXT("PlayerState missing AbilitySystemComponent"));
		return;
	}

	// Init ASC Actor Info - OwnerActor = PlayerState, AvatarActor = this
	AuraASC->InitAbilityActorInfo(AuraPS, this);

	UE_LOG(LogAuraPlayer, Log, TEXT("Player [%s] (%s) - ASC Initialized"), 
		*GetName(),
		HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));
}