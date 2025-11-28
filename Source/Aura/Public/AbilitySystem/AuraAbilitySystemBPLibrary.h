// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/AuraCharacterClassInfo.h"
#include "UI/HUD/AuraHUD.h"
#include "AuraAbilitySystemBPLibrary.generated.h"

struct FGameplayTag;
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
	static FGameplayAttribute GetResistanceAttributeForDamageType(const FGameplayTag& DamageType);
	
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
	
	UFUNCTION(BlueprintPure, Category = "Aura|BPLibrary|GameplayEffects")
	static bool IsNiceHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "Aura|BPLibrary|GameplayEffects")
	static bool IsDankHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "Aura|BPLibrary|GameplayEffects")
	static bool IsPiHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "Aura|BPLibrary|GameplayEffects")
	static void SetIsNiceHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsNiceHit);

	UFUNCTION(BlueprintCallable, Category = "Aura|BPLibrary|GameplayEffects")
	static void SetIsDankHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDankHit);

	UFUNCTION(BlueprintCallable, Category = "Aura|BPLibrary|GameplayEffects")
	static void SetIsPiHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsPiHit);
	
private:
	template <typename T>
	static T* GetWidgetController(const UObject* WorldContextObject, 
								   T* (AAuraHUD::*GetControllerFunc)(const FWidgetControllerParams&));
	
	// Helper to get Aura context from handle
	static FAuraGameplayEffectContext* GetAuraEffectContext(FGameplayEffectContextHandle& EffectContextHandle);
	static const FAuraGameplayEffectContext* GetAuraEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);
};