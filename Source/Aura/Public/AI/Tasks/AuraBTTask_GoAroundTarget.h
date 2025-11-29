// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "AuraBTTask_GoAroundTarget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBTTask_GoAroundTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	explicit UAuraBTTask_GoAroundTarget(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|AI")
	FBlackboardKeySelector NewLocation { FBlackboardKeySelector() };
};
