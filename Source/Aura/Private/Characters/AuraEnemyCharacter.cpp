// Not Sure Yet

#include "Characters/AuraEnemyCharacter.h"
#include "Aura/Aura.h"

DEFINE_LOG_CATEGORY_STATIC(LogAura, Log, All);

AAuraEnemyCharacter::AAuraEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilityRefs.InitializeComponents(this);
	AbilityRefs.SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void AAuraEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(AbilityRefs.AbilitySystemComponent);
	AbilityRefs.InitializeActorInfo(this, this);

	UE_LOG(LogAura, Log, TEXT("Enemy [%s] - ASC Initialized: %s | AttributeSet: %s"), 
		*GetName(),
		AbilityRefs.AbilitySystemComponent ? TEXT("Valid") : TEXT("NULL"),
		AbilityRefs.AttributeSet ? TEXT("Valid") : TEXT("NULL"));
}

void AAuraEnemyCharacter::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CustomDepthRed);
	
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
		Weapon->SetCustomDepthStencilValue(CustomDepthRed);
	}
}

void AAuraEnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(false);
	}
}