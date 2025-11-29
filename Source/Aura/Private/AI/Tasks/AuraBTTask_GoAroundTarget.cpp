// Not Sure Yet


#include "AI/Tasks/AuraBTTask_GoAroundTarget.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

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
	
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UAuraBTTask_GoAroundTarget, Target), AActor::StaticClass());
	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(Target.SelectedKeyName));
	
	if (!TargetActor)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	
	// Get Target Actor Location
	const FVector TargetActorLocation = TargetActor->GetActorLocation();
	
	// Get Navigation System
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(OwnerComp.GetAIOwner());
	if (!NavSys)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	// Find random navigable location around target

	if (FNavLocation RandomLoc; NavSys->GetRandomPointInNavigableRadius(TargetActorLocation, SearchRadius, RandomLoc))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(NewLocation.SelectedKeyName, RandomLoc.Location);
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}
