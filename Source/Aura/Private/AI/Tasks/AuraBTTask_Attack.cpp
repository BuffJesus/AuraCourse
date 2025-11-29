// Not Sure Yet


#include "AI/Tasks/AuraBTTask_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Tags/AuraTags.h"

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
 
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	// Get the target actor from blackboard
	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetToFollow"));
	
	// Create event data with target
	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	
	// Activate abilities with event data
	FGameplayTagContainer AbilityTags;
	AbilityTags.AddTag(Aura::Ability::Attack::Attack);
	
	ASC->TryActivateAbilitiesByTag(AbilityTags, true); 
 
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
