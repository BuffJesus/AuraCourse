// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraOverlayWidgetController.h"
#include "AuraAttributeMenuWidgetController.generated.h"

class UAuraAttributeInfo;
struct FAttributeInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAttributeInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAuraAttributeMenuWidgetController : public UAuraOverlayWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "Aura|GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAuraAttributeInfo> AttributeInfo;

private:
	// Helper function to broadcast attributes - takes Info directly (Mehmet's optimization)
	void BroadcastAttributeInfo(const FAttributeInfo& Info) const;
};