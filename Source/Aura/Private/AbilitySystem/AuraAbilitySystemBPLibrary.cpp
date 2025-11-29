// Not Sure Yet


#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "Tags/AuraTags.h"
#include "AuraAbilityTypes.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/Attributes/AuraPlayerAttributeSet.h"
#include "Interaction/AuraCombatInterface.h"
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
			if (!PS) { return nullptr; }

			UAuraAbilitySystemComponent* ASC { PS->GetAuraAbilitySystemComponent() };
			UAuraAttributeSet* AS { PS->GetAuraAttributeSet() };
			const FWidgetControllerParams Params(PC, PS, ASC, AS);
			
			// Call the member function pointer on the HUD
			return (AuraHUD->*GetControllerFunc)(Params);
		}
	}
	return nullptr;
}

FGameplayAttribute UAuraAbilitySystemBPLibrary::GetResistanceAttributeForDamageType(const FGameplayTag& DamageType)
{
	// Use a macro to create compile-time safe mappings
#define MAP_DAMAGE_TO_RESISTANCE(Type) \
if (DamageType.MatchesTagExact(Aura::Damage::Type)) \
{ \
return UAuraAttributeSet::Get##Type##ResistanceAttribute(); \
}
	
	MAP_DAMAGE_TO_RESISTANCE(Fire)
	MAP_DAMAGE_TO_RESISTANCE(Lightning)
	MAP_DAMAGE_TO_RESISTANCE(Arcane)
	MAP_DAMAGE_TO_RESISTANCE(Physical)
	
	#undef MAP_DAMAGE_TO_RESISTANCE
	
	return FGameplayAttribute();
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
	
	const AAuraGameModeBase* AuraGameMode { Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)) };
	if (!AuraGameMode) { return; }
	
	const UAuraCharacterClassInfo* CharacterClassInfo { AuraGameMode->CharacterClassInfo };
	if (!CharacterClassInfo) { return; }
	
	const FCharacterClassDefaultInfo ClassDefaultInfo { CharacterClassInfo->GetDefaultInfo(CharacterClass) };
	
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
	ApplyGameplayEffect(ClassDefaultInfo.ResistanceAttributes);
	ApplyGameplayEffect(CharacterClassInfo->VitalAttributes);
}

void UAuraAbilitySystemBPLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UAuraCharacterClassInfo* CharacterClassInfo { GetCharacterClassInfo(WorldContextObject) };
	if (!CharacterClassInfo) { return; }
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec { FGameplayAbilitySpec(AbilityClass, 1.) };
		ASC->GiveAbility(AbilitySpec);
	}
	for (const FCharacterClassDefaultInfo& DefaultInfo {CharacterClassInfo->GetDefaultInfo(CharacterClass) }; 
		TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.ClassAbilities)
	{
		if (IAuraCombatInterface* CombatInterface { Cast<IAuraCombatInterface>(ASC->GetAvatarActor()) })
		{
			FGameplayAbilitySpec AbilitySpec { FGameplayAbilitySpec(AbilityClass, CombatInterface->GetCharacterLevel()) };
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UAuraCharacterClassInfo* UAuraAbilitySystemBPLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	if (!WorldContextObject) { return nullptr; }
	
	const AAuraGameModeBase* AuraGameMode { Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)) };
	if (!AuraGameMode) { return nullptr; }
	
	return AuraGameMode->CharacterClassInfo;
}

FAuraGameplayEffectContext* UAuraAbilitySystemBPLibrary::GetAuraEffectContext(FGameplayEffectContextHandle& EffectContextHandle)
{
	return static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());
}

const FAuraGameplayEffectContext* UAuraAbilitySystemBPLibrary::GetAuraEffectContext(const FGameplayEffectContextHandle& EffectContextHandle)
{
	return static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get());
}

bool UAuraAbilitySystemBPLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		return AuraEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemBPLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		return AuraEffectContext->IsCriticalHit();
	}
	return false;
}

void UAuraAbilitySystemBPLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		AuraEffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemBPLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		AuraEffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

bool UAuraAbilitySystemBPLibrary::IsNiceHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		return AuraEffectContext->IsNiceHit();
	}
	return false;
}

bool UAuraAbilitySystemBPLibrary::IsDankHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		return AuraEffectContext->IsDankHit();
	}
	return false;
}

bool UAuraAbilitySystemBPLibrary::IsPiHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		return AuraEffectContext->IsPiHit();
	}
	return false;
}

void UAuraAbilitySystemBPLibrary::SetIsNiceHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsNiceHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		AuraEffectContext->SetIsNiceHit(bInIsNiceHit);
	}
}

void UAuraAbilitySystemBPLibrary::SetIsDankHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDankHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		AuraEffectContext->SetIsDankHit(bInIsDankHit);
	}
}

void UAuraAbilitySystemBPLibrary::SetIsPiHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsPiHit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext { GetAuraEffectContext(EffectContextHandle) })
	{
		AuraEffectContext->SetIsPiHit(bInIsPiHit);
	}
}
