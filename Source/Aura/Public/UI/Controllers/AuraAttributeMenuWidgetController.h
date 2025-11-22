// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AuraOverlayWidgetController.h"
#include "AuraAttributeMenuWidgetController.generated.h"

class UAuraAttributeInfo;
struct FAttributeInfo;
class UWidget;
class UPanelWidget;

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

	// For pre-existing row widgets: Automatically sets tags on named Row_ widgets (e.g., Row_Strength, Row_Vigor)
	UFUNCTION(BlueprintCallable, Category = "Aura|GAS|Attributes")
	void SetAttributeTagsOnExistingRows(UUserWidget* ParentWidget);

	// Populate Primary attributes (with buttons) - uses WBP_Aura_TextValueButtonRow
	UFUNCTION(BlueprintCallable, Category = "Aura|GAS|Attributes")
	void PopulatePrimaryAttributeRows(UPanelWidget* Container, TSubclassOf<UUserWidget> RowWidgetClass);

	// Populate Secondary attributes (no buttons) - uses WBP_Aura_TextValueRow
	UFUNCTION(BlueprintCallable, Category = "Aura|GAS|Attributes")
	void PopulateSecondaryAttributeRows(UPanelWidget* Container, TSubclassOf<UUserWidget> RowWidgetClass);

	// Generic function (for advanced use cases)
	UFUNCTION(BlueprintCallable, Category = "Aura|GAS|Attributes")
	void PopulateAttributeRows(UPanelWidget* Container, TSubclassOf<UUserWidget> RowWidgetClass);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAuraAttributeInfo> AttributeInfo;

private:
	// Helper function to broadcast attributes - takes Info directly (Mehmet's optimization)
	void BroadcastAttributeInfo(const FAttributeInfo& Info) const;
	
	// Helper to populate with filter
	void PopulateAttributeRowsInternal(UPanelWidget* Container, TSubclassOf<UUserWidget> RowWidgetClass, 
		const FGameplayTag& FilterTag);
};