// Not Sure Yet
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AuraBTS_FindNearestPlayer.generated.h"

UCLASS()
class AURA_API UAuraBTS_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()
	
public:
	UAuraBTS_FindNearestPlayer();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector NearestPlayerSelector { FBlackboardKeySelector() };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector NearestPlayerDistanceSelector { FBlackboardKeySelector() };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector HasNearestPlayerSelector { FBlackboardKeySelector() };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI", meta = (ClampMin = "0.0"))
	float MaxSearchDistance { 2000.f };
};