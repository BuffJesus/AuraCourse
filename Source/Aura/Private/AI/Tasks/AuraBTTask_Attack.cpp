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
	UE_LOG(LogTemp, Warning, TEXT("=== AuraBTTask_Attack::ExecuteTask START ==="));
	
	if (!OwnerComp.GetBlackboardComponent() ||
	  !OwnerComp.GetAIOwner() ||
	  !IsValid(OwnerComp.GetAIOwner()->GetPawn()))
	{
		UE_LOG(LogTemp, Error, TEXT("AuraBTTask_Attack - Invalid components"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
 
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	UE_LOG(LogTemp, Log, TEXT("AuraBTTask_Attack - AI Pawn: %s"), *AIPawn->GetName());
	
	// Get the ability system component from the AI pawn
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIPawn);
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("AuraBTTask_Attack - No ASC found on AI Pawn"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	
	UE_LOG(LogTemp, Log, TEXT("AuraBTTask_Attack - ASC found"));
	
	// Get the target actor from blackboard
	AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetToFollow"));
	
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("AuraBTTask_Attack - No target in blackboard"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AuraBTTask_Attack - Target: %s at location: %s"), 
			*TargetActor->GetName(), 
			*TargetActor->GetActorLocation().ToString());
	}
	
	// Create event data with target
	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	
	UE_LOG(LogTemp, Log, TEXT("AuraBTTask_Attack - Sending HandleGameplayEvent with tag: %s"), 
	*FGameplayTag(Aura::Ability::Attack::Attack).ToString());
	
	// Send a gameplay event with the attack tag and target data
	ASC->HandleGameplayEvent(Aura::Ability::Attack::Attack, &EventData);
 
	UE_LOG(LogTemp, Warning, TEXT("=== AuraBTTask_Attack::ExecuteTask END ==="));
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
