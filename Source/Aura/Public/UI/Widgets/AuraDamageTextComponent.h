// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AuraDamageTextComponent.generated.h"

UENUM(BlueprintType)
enum class EAuraHitType : uint8
{
	Normal      UMETA(DisplayName = "Normal"),
	Blocked     UMETA(DisplayName = "Blocked"),
	Critical    UMETA(DisplayName = "Critical"),
	Nice        UMETA(DisplayName = "Nice"),
	Dank        UMETA(DisplayName = "Dank"),
	Pi          UMETA(DisplayName = "Pi"),
	CriticalBlocked UMETA(DisplayName = "Critical Block")
};

UCLASS()
class AURA_API UAuraDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UAuraDamageTextComponent();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Aura|UI")
	void SetDamageText(float Damage, bool bBlockedHit, bool bCriticalHit, bool bNiceHit, bool bDankHit, bool bPiHit);
	
	// Helper function to determine hit type from booleans
	UFUNCTION(BlueprintPure, Category = "Aura|UI")
	static EAuraHitType DetermineHitType(bool bBlocked, bool bCritical, bool bNice, bool bDank, bool bPi);
	
	// Helper to get display text for hit type
	UFUNCTION(BlueprintPure, Category = "Aura|UI")
	static FText GetHitTypeText(EAuraHitType HitType);
};