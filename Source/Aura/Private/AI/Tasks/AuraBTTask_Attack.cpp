// Not Sure Yet


#include "AI/Tasks/AuraBTTask_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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
 
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	
	// Get the ability system component from the AI pawn
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIPawn);
	if (!ASC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}
	
	// Create a tag container with the abilities you want to activate
	FGameplayTagContainer AbilityTags;
	// Add your attack ability tag(s) here, for example:
	// AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Attack")));
	
	// Try to activate abilities by tag
	ASC->TryActivateAbilitiesByTag(AbilityTags);
 
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
