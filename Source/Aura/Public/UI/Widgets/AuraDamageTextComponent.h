// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AuraDamageTextComponent.generated.h"


UCLASS()
class AURA_API UAuraDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UAuraDamageTextComponent();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Aura|UI")
	void SetDamageText(float Damage, bool bBlockedHit, bool bCriticalHit, bool bNiceHit, bool bDankHit, bool bPiHit);
};
