// Not Sure Yet

#include "AbilitySystem/Attributes/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "Interaction/AuraCombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"
#include "Tags/AuraTags.h"

#pragma region Macros
// Implements the OnRep function for an attribute
#define IMPLEMENT_ATTRIBUTE_ONREP(ClassName, AttributeName) \
void ClassName::OnRep_##AttributeName(const FGameplayAttributeData& Old##AttributeName) const \
{ \
GAMEPLAYATTRIBUTE_REPNOTIFY(ClassName, AttributeName, Old##AttributeName); \
}

// Registers an attribute for replication
#define REPLICATE_ATTRIBUTE(ClassName, AttributeName) \
DOREPLIFETIME_CONDITION_NOTIFY(ClassName, AttributeName, COND_None, REPNOTIFY_Always)
#pragma endregion

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, Strength);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, Intelligence);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, Resilience);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, Vigor);
	
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, Armor);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, ArmorPenetration);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, BlockChance);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, CriticalHitChance);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, CriticalHitDamage);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, CriticalHitResistance);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, HealthRegeneration);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, ManaRegeneration);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, MaxHealth);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, MaxMana);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, Luck);
	
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, FireResistance);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, LightningResistance);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, ArcaneResistance);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, PhysicalResistance);
	
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, Health);
	REPLICATE_ATTRIBUTE(UAuraAttributeSet, Mana);
}

void UAuraAttributeSet::ClampVitalAttribute(const FGameplayAttribute& VitalAttribute, float& NewValue) const
{
	// Lambda to clamp a vital attribute to its max value
	auto ClampToMax = [&NewValue](const float MaxValue)
	{
		if (MaxValue > 0.f)
		{
			NewValue = FMath::Clamp(NewValue, 0.f, MaxValue);
		}
	};
	
	if (VitalAttribute == GetHealthAttribute())
	{
		ClampToMax(GetMaxHealth());
	}
	else if (VitalAttribute == GetManaAttribute())
	{
		ClampToMax(GetMaxMana());
	}
}

void UAuraAttributeSet::ClampMaxAttribute(const FGameplayAttribute& MaxAttribute, float& NewValue) const
{
	if (MaxAttribute == GetMaxHealthAttribute() || MaxAttribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UAuraAttributeSet::ClampCurrentVitalAttributes()
{
	// Lambda to clamp a vital stat to its max
	auto ClampVital = [](float& CurrentValue, const float MaxValue)
	{
		if (MaxValue > 0.f)
		{
			CurrentValue = FMath::Clamp(CurrentValue, 0.f, MaxValue);
		}
	};
	
	float CurrentHealth { GetHealth() };
	float CurrentMana { GetMana() };
	
	ClampVital(CurrentHealth, GetMaxHealth());
	ClampVital(CurrentMana, GetMaxMana());
	
	SetHealth(CurrentHealth);
	SetMana(CurrentMana);
}

void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampVitalAttribute(Attribute, NewValue);
	ClampMaxAttribute(Attribute, NewValue);
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	const FGameplayAttribute& ModifiedAttribute { Data.EvaluatedData.Attribute };

	// Lambda for clamping a vital attribute (Health or Mana) to its max
	auto ClampVitalToMax = [this](const bool bIsHealth)
	{
		const float CurrentValue { bIsHealth ? GetHealth() : GetMana() };
		const float MaxValue { bIsHealth ? GetMaxHealth() : GetMaxMana() };
		
		if (MaxValue > 0.f)
		{
			const float ClampedValue { FMath::Clamp(CurrentValue, 0.f, MaxValue) };
			bIsHealth ? SetHealth(ClampedValue) : SetMana(ClampedValue);
		}
	};
	
	// When Health or Mana is modified directly, clamp to max
	if (ModifiedAttribute == GetHealthAttribute())
	{
		ClampVitalToMax(true);
	}
	else if (ModifiedAttribute == GetManaAttribute())
	{
		ClampVitalToMax(false);
	}

	// When MaxHealth or MaxMana changes, re-clamp current values
	if (ModifiedAttribute == GetMaxHealthAttribute() || ModifiedAttribute == GetMaxManaAttribute())
	{
		ClampCurrentVitalAttributes();
	}
	
	if (ModifiedAttribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(Aura::Effects::HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}
        
			// Extract hit type from custom effect context
			const bool bBlocked = UAuraAbilitySystemBPLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool bCriticalHit = UAuraAbilitySystemBPLibrary::IsCriticalHit(Props.EffectContextHandle);

			// Check for meme damage values
			const bool bIsNiceHit = FMath::IsNearlyEqual(LocalIncomingDamage, 69.f, 0.01f);
			const bool bIsDankHit = FMath::IsNearlyEqual(LocalIncomingDamage, 420.f, 0.01f);
			const bool bIsPiHit = FMath::IsNearlyEqual(LocalIncomingDamage, 3.14f, 0.01f) || 
								  FMath::IsNearlyEqual(LocalIncomingDamage, PI, 0.01f);

			// Set the meme flags in the context for replication
			if (bIsNiceHit)
			{
				UAuraAbilitySystemBPLibrary::SetIsNiceHit(Props.EffectContextHandle, true);
			}
			if (bIsDankHit)
			{
				UAuraAbilitySystemBPLibrary::SetIsDankHit(Props.EffectContextHandle, true);
			}
			if (bIsPiHit)
			{
				UAuraAbilitySystemBPLibrary::SetIsPiHit(Props.EffectContextHandle, true);
			}

			// Setup GameplayCue parameters with hit type information
			FGameplayCueParameters CueParams;
			CueParams.RawMagnitude = LocalIncomingDamage;
			CueParams.EffectContext = Props.EffectContextHandle;

			Props.TargetASC->ExecuteGameplayCue(Aura::GameplayCue::Aura::DamageText, CueParams);
		}
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, Strength)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, Intelligence)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, Resilience)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, Vigor)

IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, Armor)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, ArmorPenetration)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, BlockChance)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, CriticalHitChance)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, CriticalHitDamage)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, CriticalHitResistance)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, HealthRegeneration)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, ManaRegeneration)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, MaxHealth)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, MaxMana)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, Luck)

IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, FireResistance)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, LightningResistance)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, ArcaneResistance)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, PhysicalResistance)

IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, Health)
IMPLEMENT_ATTRIBUTE_ONREP(UAuraAttributeSet, Mana)