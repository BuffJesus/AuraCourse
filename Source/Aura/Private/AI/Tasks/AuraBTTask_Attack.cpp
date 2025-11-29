// Not Sure Yet


#include "AI/Tasks/AuraBTTask_Attack.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UAuraBTTask_Attack::UAuraBTTask_Attack(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UAuraBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!OwnerComp.GetBlackboardComponent() ||
	  !OwnerComp.GetAIOwner() ||
	  !IsValid(OwnerComp.GetAIOwner()->GetPawn()))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
 
	const FVector ActorLocation = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();
	DrawDebugSphere(OwnerComp.GetBlackboardComponent()->GetOwner()->GetWorld(), ActorLocation, 40.f, 12.f, FColor::Red, false, 3.f);
 
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
