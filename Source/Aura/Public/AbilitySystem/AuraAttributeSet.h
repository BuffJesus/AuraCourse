// Not Sure Yet

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

UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

#pragma region Primary Attribute Declaration
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Aura|Attributes|Primary")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, Strength);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Aura|Attributes|Primary")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, Intelligence);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Aura|Attributes|Primary")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, Resilience);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Aura|Attributes|Primary")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, Vigor);
#pragma endregion

#pragma region Secondary Attribute Declaration
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, ArmorPenetration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, BlockChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, CriticalHitChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, CriticalHitDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, CriticalHitResistance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, HealthRegeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, ManaRegeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Aura|Attributes|Secondary")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, MaxMana);
#pragma endregion

#pragma region Vital Attribute Declaration
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Aura|Attributes|Vital")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Aura|Attributes|Vital")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS_BASIC(UAuraAttributeSet, Mana);
#pragma endregion

#pragma region Primary Attribute Replication
	UFUNCTION() void OnRep_Strength(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_Intelligence(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_Resilience(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_Vigor(const FGameplayAttributeData& Old) const;
#pragma endregion

#pragma region Secondary Attribute Replication
	UFUNCTION() void OnRep_Armor(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_ArmorPenetration(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_BlockChance(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_CriticalHitChance(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_CriticalHitDamage(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_CriticalHitResistance(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_HealthRegeneration(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_ManaRegeneration(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_MaxHealth(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_MaxMana(const FGameplayAttributeData& Old) const;
#pragma endregion

#pragma region Vital Attribute Replication
	UFUNCTION() void OnRep_Health(const FGameplayAttributeData& Old) const;
	UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& Old) const;
#pragma endregion

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;

	// Helper functions for attribute clamping
	void ClampVitalAttribute(const FGameplayAttribute& VitalAttribute, float& NewValue) const;
	void ClampMaxAttribute(const FGameplayAttribute& MaxAttribute, float& NewValue) const;
	void ClampCurrentVitalAttributes();
};