// Not Sure Yet


#include "AI/Tasks/AuraBTS_FindNearestPlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UAuraBTS_FindNearestPlayer::UAuraBTS_FindNearestPlayer()
{
	INIT_SERVICE_NODE_NOTIFY_FLAGS();
	NodeName = TEXT("Find Nearest Player");
	
}
void UAuraBTS_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) { return; }
	AActor* NearestPlayer { nullptr };
	float Distance { 0.f };
	bool bHasNearestPlayer { false };
	Blackboard->SetValue<UBlackboardKeyType_Float>(TEXT("NearestPlayerDistance"), Distance);
	Blackboard->SetValue<UBlackboardKeyType_Object>(TEXT("NearestPlayer"), NearestPlayer);
	Blackboard->SetValue<UBlackboardKeyType_Bool>(TEXT("HasNearestPlayer"), bHasNearestPlayer);
}
