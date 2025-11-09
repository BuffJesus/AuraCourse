// Not Sure Yet


#include "Characters/AuraEnemyCharacter.h"


AAuraEnemyCharacter::AAuraEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAuraEnemyCharacter::HighlightActor()
{
	bIsHighlighted = true;
}

void AAuraEnemyCharacter::UnHighlightActor()
{
	bIsHighlighted = false;
}

void AAuraEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsHighlighted && bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), 100.0f, 16, FColor::Red, false, 0.0f, 0, 1.0f);
	}
}

void AAuraEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}
