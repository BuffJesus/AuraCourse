// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AuraTargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, Handle);

/**
 * 
 */
UCLASS()
class AURA_API UAuraTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Aura|Tasks", meta = (DisplayName = "TargetDataUnderMouse", 
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAuraTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);
	
	UPROPERTY(BlueprintAssignable, Category = "Aura|Tasks")
	FMouseTargetDataSignature ValidData;
	
private:
	virtual void Activate() override;
	void SendCursorData();
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Handle, FGameplayTag ActivationTag);
};
