// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC { nullptr };

	UPROPERTY()
	AActor* SourceAvatarActor { nullptr };

	UPROPERTY()
	AController* SourceController { nullptr };

	UPROPERTY()
	ACharacter* SourceCharacter { nullptr };

	UPROPERTY()
	UAbilitySystemComponent* TargetASC { nullptr };

	UPROPERTY()
	AActor* TargetAvatarActor { nullptr };

	UPROPERTY()
	AController* TargetController { nullptr };

	UPROPERTY()
	ACharacter* TargetCharacter { nullptr };
};

// ATTRIBUTE MACROS
// These macros reduce boilerplate for declaring replicated attributes.
// Usage: DECLARE_ATTRIBUTE(AttributeName, Category);  // Semicolon required

// Declares a replicated attribute with accessors
#define DECLARE_ATTRIBUTE(AttributeName, Category) \
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_##AttributeName, Category = Category) \
	FGameplayAttributeData AttributeName; \
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, AttributeName); 

// Declares the OnRep function signature
#define DECLARE_ATTRIBUTE_ONREP(AttributeName) \
	UFUNCTION() \
	void OnRep_##AttributeName(const FGameplayAttributeData& Old##AttributeName) const

// Implements the OnRep function
#define IMPLEMENT_ATTRIBUTE_ONREP(ClassName, AttributeName) \
void ClassName::OnRep_##AttributeName(const FGameplayAttributeData& Old##AttributeName) const \
{ \
	GAMEPLAYATTRIBUTE_REPNOTIFY(ClassName, AttributeName, Old##AttributeName); \
}

// Registers attribute for replication
#define REPLICATE_ATTRIBUTE(ClassName, AttributeName) \
	DOREPLIFETIME_CONDITION_NOTIFY(ClassName, AttributeName, COND_None, REPNOTIFY_Always)
// ============================================================================

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	DECLARE_ATTRIBUTE(Strength, "Aura|Attributes|Primary")
	DECLARE_ATTRIBUTE(Intelligence, "Aura|Attributes|Primary")
	DECLARE_ATTRIBUTE(Resilience, "Aura|Attributes|Primary")
	DECLARE_ATTRIBUTE(Vigor, "Aura|Attributes|Primary")
	
	DECLARE_ATTRIBUTE(Health, "Aura|Attributes|Vital")
	DECLARE_ATTRIBUTE(MaxHealth, "Aura|Attributes|Vital")
	DECLARE_ATTRIBUTE(Mana, "Aura|Attributes|Vital")
	DECLARE_ATTRIBUTE(MaxMana, "Aura|Attributes|Vital")
	
	DECLARE_ATTRIBUTE_ONREP(Strength);
	DECLARE_ATTRIBUTE_ONREP(Intelligence);
	DECLARE_ATTRIBUTE_ONREP(Resilience);
	DECLARE_ATTRIBUTE_ONREP(Vigor);
	
	DECLARE_ATTRIBUTE_ONREP(Health);
	DECLARE_ATTRIBUTE_ONREP(MaxHealth);
	DECLARE_ATTRIBUTE_ONREP(Mana);
	DECLARE_ATTRIBUTE_ONREP(MaxMana);

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};