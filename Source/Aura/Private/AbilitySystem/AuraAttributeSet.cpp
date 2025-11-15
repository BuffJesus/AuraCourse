// Not Sure Yet


#include "AbilitySystem/AuraAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(12.f);
	InitMaxHealth(100.f);
	InitMana(12.f);
	InitMaxMana(100.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

#pragma region DOREPLIFETIME VitalAttributes	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMana, COND_None, REPNOTIFY_Always);
#pragma endregion
	
}

void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// Clamp Health to [0, MaxHealth] if MaxHealth is initialized
	if (Attribute == GetHealthAttribute())
	{
		if (GetMaxHealth() > 0.f)
		{
			NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
		}
	}

	// Clamp Mana to [0, MaxMana] if MaxMana is initialized
	if (Attribute == GetManaAttribute())
	{
		if (GetMaxMana() > 0.f)
		{
			NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
		}
	}

	// Clamp max values to be non-negative
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}

	if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.f);
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Clamp Health to [0, MaxHealth] if MaxHealth is initialized
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (GetMaxHealth() > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		}
	}

	// Clamp Mana to [0, MaxMana] if MaxMana is initialized
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		if (GetMaxMana() > 0.f)
		{
			SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
		}
	}

	// Also clamp current values when max values change
	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		if (GetMaxHealth() > 0.f)
		{
			SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
		}
	}

	if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
	{
		if (GetMaxMana() > 0.f)
		{
			SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
		}
	}
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMana, OldMaxMana);
}
