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

 // Safety clamps for final critical damage multiplier after bonuses
 UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|CriticalHit",
     meta = (ToolTip = "Minimum allowed final crit multiplier after bonuses (e.g., 1.0 = no less than normal damage)"))
 float MinCriticalHitMultiplier { 1.0f };

 UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|CriticalHit",
     meta = (ToolTip = "Maximum allowed final crit multiplier after bonuses (e.g., 3.0 caps extreme stacking)"))
 float MaxCriticalHitMultiplier { 3.0f };
	
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

    // Toggle detailed damage/crit debug logs at runtime without code changes
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Advanced",
        meta = (ToolTip = "Enable verbose damage calculation logs (including crit details)") )
    bool bEnableDamageDebugLogs { true };
};
