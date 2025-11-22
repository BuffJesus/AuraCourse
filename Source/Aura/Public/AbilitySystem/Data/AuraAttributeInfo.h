// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraAttributeInfo.generated.h"

struct FGameplayAttribute;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayAttribute AttributeGetter;
};

UCLASS()
class AURA_API UAuraAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;

#if WITH_EDITOR
	// Button to automatically populate DataAsset from native tags
	UFUNCTION(CallInEditor, Category = "Populate Attribute Info")
	void PopulateDataAsset();
#endif
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "{AttributeName}"))
	TArray<FAttributeInfo> AttributeInfo;
};