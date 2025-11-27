// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/AuraCharacterClassInfo.h"
#include "UI/HUD/AuraHUD.h"
#include "AuraAbilitySystemBPLibrary.generated.h"

struct FAuraGameplayEffectContext;
struct FGameplayEffectContextHandle;
struct FWidgetControllerParams;
class UAuraAttributeMenuWidgetController;
class UAuraOverlayWidgetController;
class UAuraWidgetController;


UCLASS()
class AURA_API UAuraAbilitySystemBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Aura|BPLibrary|WidgetController", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UAuraOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Aura|BPLibrary|WidgetController", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UAuraAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "Aura|BPLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "Aura|BPLibrary|CharacterClassDefaults")
	static UAuraCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "Aura|BPLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "Aura|BPLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintCallable, Category = "Aura|BPLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
	
	UFUNCTION(BlueprintCallable, Category = "Aura|BPLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
	
private:
	template <typename T>
	static T* GetWidgetController(const UObject* WorldContextObject, 
								   T* (AAuraHUD::*GetControllerFunc)(const FWidgetControllerParams&));
	
	// Helper to get Aura context from handle
	static FAuraGameplayEffectContext* GetAuraEffectContext(FGameplayEffectContextHandle& EffectContextHandle);
	static const FAuraGameplayEffectContext* GetAuraEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);
};