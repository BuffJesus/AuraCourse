// Not Sure Yet


#include "AI/Tasks/AuraBTTask_GoAroundTarget.h"
#include "AIController.h"

UAuraBTTask_GoAroundTarget::UAuraBTTask_GoAroundTarget(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("GoAroundTarget");
}

EBTNodeResult::Type UAuraBTTask_GoAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!OwnerComp.GetBlackboardComponent() ||
	  !OwnerComp.GetAIOwner() ||
	  !IsValid(OwnerComp.GetAIOwner()->GetPawn()))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
