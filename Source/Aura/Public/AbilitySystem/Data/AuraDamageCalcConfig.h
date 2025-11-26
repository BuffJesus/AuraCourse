// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AuraDamageCalcConfig.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class AURA_API UAuraDamageCalcConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Block")
	float BlockDamageReduction { 0.5f };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Armor")
	FName ArmorPenCurveName { "ArmorPenetration" };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Armor")
	FName EffectiveArmorCurveName { "EffectiveArmor" };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|CriticalHit")
	float CriticalHitMultiplier { 2.f };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|CriticalHit")
	FName CriticalHitResistanceCurveName { "CriticalHitResistance" };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Luck", 
		meta = (ToolTip = "How much luck increases armor penetration (1:1 ratio by default)"))
	float LuckToArmorPenetrationRatio { 1.0f };
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Luck",
		meta = (ToolTip = "How much luck increases critical hit chance (1:1 ratio by default)"))
	float LuckToCriticalHitChanceRatio { 1.0f };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Advanced")
	bool bLuckAffectsArmorPenetration { true };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Advanced")
	bool bLuckAffectsCriticalHitChance { true };
};
