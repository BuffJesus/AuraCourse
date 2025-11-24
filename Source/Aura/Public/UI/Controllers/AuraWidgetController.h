
// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AuraWidgetController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraAttributeSet;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	// Keep base types for flexibility
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAuraAbilitySystemComponent* ASC, UAuraAttributeSet* AS) 
		: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<APlayerController> PlayerController { nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<APlayerState> PlayerState { nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent { nullptr };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAuraAttributeSet> AttributeSet { nullptr };
};

/**
 * Base widget controller class
 */
UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& Params);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();
	
	/** Get typed AbilitySystemComponent - caches cast on first call */
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;
	
	/** Get typed AttributeSet - caches cast on first call */
	UAuraAttributeSet* GetAuraAttributeSet() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Aura|WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Aura|WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "Aura|WidgetController")
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Aura|WidgetController")
	TObjectPtr<UAuraAttributeSet> AttributeSet;

private:
	/** Cached typed components - initialized on first access */
	mutable TObjectPtr<UAuraAbilitySystemComponent> CachedAuraASC;
	mutable TObjectPtr<UAuraAttributeSet> CachedAuraAttributeSet;
};