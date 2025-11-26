// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AuraCharacterClassInfo.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	DefaultClass,
	Elementalist,
	Warrior,
	Ranger
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|ClassDefaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|ClassDefaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Aura|CharacterClassDefaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInfo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|CommonClassDefaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category = "Aura|CommonClassDefaults")
	TObjectPtr<UCurveTable> DamageCalcCoefficients;
	
	FORCEINLINE FCharacterClassDefaultInfo GetDefaultInfo(const ECharacterClass Class) const
	{
		return CharacterClassInfo.FindRef(Class);
	};
};
