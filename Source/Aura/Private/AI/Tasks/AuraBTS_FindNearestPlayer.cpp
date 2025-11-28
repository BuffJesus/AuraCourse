// AuraBTS_FindNearestPlayer.cpp
#include "AI/Tasks/AuraBTS_FindNearestPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UAuraBTS_FindNearestPlayer::UAuraBTS_FindNearestPlayer()
{
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	NodeName = TEXT("Find Nearest Player");
	
	// Set default interval for service tick
	Interval = 0.5f;
	RandomDeviation = 0.1f;
	
	NearestPlayerSelector.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UAuraBTS_FindNearestPlayer, NearestPlayerSelector), AActor::StaticClass());
	NearestPlayerDistanceSelector.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UAuraBTS_FindNearestPlayer, NearestPlayerDistanceSelector));
	HasNearestPlayerSelector.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UAuraBTS_FindNearestPlayer, HasNearestPlayerSelector));
}

void UAuraBTS_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	APawn* OwningPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!OwningPawn) { return; }

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) { return; }

	// Find all player controllers in the world
	TArray<AActor*> PlayerCharacters{};
	UGameplayStatics::GetAllActorsWithTag(OwningPawn->GetWorld(), FName("Player"), PlayerCharacters);

	AActor* NearestPlayer{};
	float NearestDistance{TNumericLimits<float>::Max()};
	
	for (AActor* PlayerActor : PlayerCharacters)
	{
		if (!IsValid(PlayerActor)) { continue; }
		
		const float Distance{OwningPawn->GetDistanceTo(PlayerActor)};
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestPlayer = PlayerActor;
		}
	}

	const bool bHasNearestPlayer{IsValid(NearestPlayer)};
	
	BlackboardComp->SetValueAsObject(NearestPlayerSelector.SelectedKeyName, NearestPlayer);
	BlackboardComp->SetValueAsFloat(NearestPlayerDistanceSelector.SelectedKeyName, bHasNearestPlayer ? NearestDistance : 0.f);
	BlackboardComp->SetValueAsBool(HasNearestPlayerSelector.SelectedKeyName, bHasNearestPlayer);
}