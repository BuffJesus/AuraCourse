// Not Sure Yet

#include "Characters/AuraPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogAura, Log, All);

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
}

void AAuraPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitializeAbilityActorInfo();
}

void AAuraPlayerCharacter::InitializeAbilityActorInfo()
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>();
	check(AuraPS);

	// Set the ASC from PlayerState (PlayerState owns it for players)
	AbilityRefs.AbilitySystemComponent = AuraPS->GetAbilitySystemComponent();
	AbilityRefs.AttributeSet = AuraPS->GetAttributeSet();

	// Init ASC Actor Info - OwnerActor = PlayerState, AvatarActor = this
	AbilityRefs.InitializeActorInfo(AuraPS, this);

	UE_LOG(LogAura, Log, TEXT("Player [%s] (%s) - ASC Initialized: %s | AttributeSet: %s"), 
		*GetName(),
		HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"),
		AbilityRefs.AbilitySystemComponent ? TEXT("Valid") : TEXT("NULL"),
		AbilityRefs.AttributeSet ? TEXT("Valid") : TEXT("NULL"));
}