// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "AuraCombatInterface.generated.h"

class AActor;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UAuraCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IAuraCombatInterface
{
        GENERATED_BODY()

        // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
        virtual int32 GetCharacterLevel() const;
        virtual FVector GetCombatSocketLocation() const;
        virtual FVector GetCombatSocketLocationByTag(const FGameplayTag& SocketTag) const;
        virtual AActor* GetCombatTarget() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	void UpdateFacingTarget(const FVector& Target);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	UAnimMontage* GetHitReactMontage() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Aura|Combat")
	UAnimMontage* GetAttackMontage() const;
	
        virtual void Die() = 0;

};
