// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AuraBTS_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBTS_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()
	
public:
	UAuraBTS_FindNearestPlayer();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
