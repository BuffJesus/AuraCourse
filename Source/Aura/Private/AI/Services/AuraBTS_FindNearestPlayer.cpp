// AuraBTS_FindNearestPlayer.cpp
#include "AI/Services/AuraBTS_FindNearestPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UAuraBTS_FindNearestPlayer::UAuraBTS_FindNearestPlayer()
{
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	NodeName = TEXT("FindNearestPlayer");
	
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
	
	APawn* OwningPawn { OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr };
	if (!OwningPawn) { return; }

	UBlackboardComponent* BlackboardComp { OwnerComp.GetBlackboardComponent() };
	if (!BlackboardComp) { return; }

	AActor* NearestPlayer{};
	float NearestDistance{TNumericLimits<float>::Max()};

	for (FConstPlayerControllerIterator It{OwningPawn->GetWorld()->GetPlayerControllerIterator()}; It; ++It)
	{
		if (APlayerController* PC { It->Get() })
		{
			if (APawn* PlayerPawn{PC->GetPawn()})
			{
				const float Distance{OwningPawn->GetDistanceTo(PlayerPawn)};
				if (Distance < NearestDistance && Distance <= MaxSearchDistance)  // Add max distance check
				{
					NearestDistance = Distance;
					NearestPlayer = PlayerPawn;
				}
			}
		}
	}

	const bool bHasNearestPlayer{IsValid(NearestPlayer)};

	BlackboardComp->SetValueAsObject(NearestPlayerSelector.SelectedKeyName, NearestPlayer);
	BlackboardComp->SetValueAsFloat(NearestPlayerDistanceSelector.SelectedKeyName, bHasNearestPlayer ? NearestDistance : 0.f);
	BlackboardComp->SetValueAsBool(HasNearestPlayerSelector.SelectedKeyName, bHasNearestPlayer);
}