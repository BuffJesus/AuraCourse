// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraOverlayWidgetController.h"
#include "AuraAttributeMenuWidgetController.generated.h"

class UAuraAttributeInfo;
struct FAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAttributeInfo&, Info);

/**
 * Widget controller for attribute menu - handles broadcasting attribute changes
 * and setting tags on pre-existing row widgets
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

	/**
	 * Automatically sets attribute tags on existing row widgets by matching widget names to attribute names
	 * Expected naming convention: Row_<AttributeName> (e.g., Row_Strength, Row_Vigor)
	 * The widget must implement SetAttributeTag(FGameplayTag) function
	 */
	UFUNCTION(BlueprintCallable, Category = "Aura|GAS|Attributes")
	void SetAttributeTagsOnExistingRows(UAuraUserWidget* ParentWidget);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAuraAttributeInfo> AttributeInfo;

private:
	// Helper function to broadcast attribute info with current values
	void BroadcastAttributeInfo(const FAttributeInfo& Info) const;
};