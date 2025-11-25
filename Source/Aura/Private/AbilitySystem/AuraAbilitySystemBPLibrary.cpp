// Not Sure Yet


#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraPlayerAttributeSet.h"
#include "UI/Controllers/AuraAttributeMenuWidgetController.h"
#include "UI/Controllers/AuraOverlayWidgetController.h"
#include "UI/Controllers/AuraWidgetController.h"
#include "UI/HUD/AuraHUD.h"


template <typename T>
T* UAuraAbilitySystemBPLibrary::GetWidgetController(const UObject* WorldContextObject,
													 T* (AAuraHUD::*GetControllerFunc)(const FWidgetControllerParams&))
{
	if (!WorldContextObject) { return nullptr; }
	
	if (APlayerController* PC { WorldContextObject->GetWorld()->GetFirstPlayerController() })
	{
		if (AAuraHUD* AuraHUD { Cast<AAuraHUD>(PC->GetHUD()) })
		{
			AAuraPlayerState* PS { PC->GetPlayerState<AAuraPlayerState>() };
			UAuraAbilitySystemComponent* ASC { PS->GetAuraAbilitySystemComponent() };
			UAuraAttributeSet* AS { PS->GetAuraAttributeSet() };
			const FWidgetControllerParams Params(PC, PS, ASC, AS);
			
			// Call the member function pointer on the HUD
			return (AuraHUD->*GetControllerFunc)(Params);
		}
	}
	return nullptr;
}

UAuraOverlayWidgetController* UAuraAbilitySystemBPLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	return GetWidgetController<UAuraOverlayWidgetController>(WorldContextObject, &AAuraHUD::GetOverlayWidgetController);
}

UAuraAttributeMenuWidgetController* UAuraAbilitySystemBPLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	return GetWidgetController<UAuraAttributeMenuWidgetController>(WorldContextObject, &AAuraHUD::GetAttributeMenuWidgetController);
}

void UAuraAbilitySystemBPLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, 
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	if (!ASC || !WorldContextObject) { return; }
	
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode) { return; }
	
	const UAuraCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	if (!CharacterClassInfo) { return; }
	
	const FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetDefaultInfo(CharacterClass);
	
	// Helper lambda to apply gameplay effects
	auto ApplyGameplayEffect = [&](const TSubclassOf<UGameplayEffect>& EffectClass)
	{
		if (!EffectClass) { return; }
		
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(ASC->GetAvatarActor());
		const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, Level, ContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	};
	
	// Apply all attribute effects
	ApplyGameplayEffect(ClassDefaultInfo.PrimaryAttributes);
	ApplyGameplayEffect(ClassDefaultInfo.SecondaryAttributes);
	ApplyGameplayEffect(CharacterClassInfo->VitalAttributes);
}
