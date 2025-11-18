// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "AuraOverlayWidgetController.generated.h"

struct FGameplayAttribute;
class UAuraUserWidget;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag { FGameplayTag() };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message { FText() };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image { nullptr };
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow,Row);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAuraOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "Aura|GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Aura|GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Aura|GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Aura|GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Aura|GAS|Attributes")
	FMessageWidgetRowSignature OnMessageWidgetRowChanged;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|WidgetData")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	template<typename TDelegate>
	void BindAttributeChangeDelegate(const FGameplayAttribute& Attribute, TDelegate& Delegate);
};

template <typename T>
T* UAuraOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""), false);
}

template <typename TDelegate>
void UAuraOverlayWidgetController::BindAttributeChangeDelegate(const FGameplayAttribute& Attribute, TDelegate& Delegate)
{
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute)
		.AddLambda([&Delegate](const FOnAttributeChangeData& Data)
		{
			Delegate.Broadcast(Data.NewValue);
		});
}
