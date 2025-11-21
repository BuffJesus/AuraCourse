// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraOverlayWidgetController.h"
#include "AuraAttributeMenuWidgetController.generated.h"

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
};
