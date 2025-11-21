// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraAttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag { FGameplayTag() };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName { FText() };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription { FText() };

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue { 0.f };
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound= false) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "{AttributeName}"))
	TArray<FAttributeInfo> AttributeInfo;
};
