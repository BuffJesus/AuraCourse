// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "AuraBTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBTTask_Attack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	explicit UAuraBTTask_Attack(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
