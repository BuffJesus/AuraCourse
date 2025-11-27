// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};

/**
 * Struct to hold SetByCaller magnitude data
 */
USTRUCT(BlueprintType)
struct FSetByCallerMagnitude
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnitude")
	FGameplayTag DataTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magnitude")
	float Magnitude { 0.f };
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	FActiveGameplayEffectHandle ApplyEffectToTarget(
		AActor* TargetActor, 
		TSubclassOf<UGameplayEffect> EffectClass,
		const TArray<FSetByCallerMagnitude>& SetByCallerMagnitudes  
	);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	bool bDestroyOnEffectRemoval { false };

	// Instant Effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> InstantGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy { EEffectApplicationPolicy::DoNotApply };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Effects")
	TArray<FSetByCallerMagnitude> InstantSetByCallerMagnitudes;

	// Duration Effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> DurationGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy { EEffectApplicationPolicy::DoNotApply };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Effects")
	TArray<FSetByCallerMagnitude> DurationSetByCallerMagnitudes;

	// Periodic Effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> PeriodicGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectApplicationPolicy PeriodicEffectApplicationPolicy { EEffectApplicationPolicy::DoNotApply };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Effects")
	TArray<FSetByCallerMagnitude> PeriodicSetByCallerMagnitudes;

	// Infinite Effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> InfiniteGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy { EEffectApplicationPolicy::DoNotApply };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy { EEffectRemovalPolicy::RemoveOnEndOverlap };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Effects")
	TArray<FSetByCallerMagnitude> InfiniteSetByCallerMagnitudes;

	// Level to apply effects at (for scaling)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Effects", meta = (ClampMin = "1.0"))
	float ActorLevel { 1.0f };

	// Track infinite effect handles per overlapping actor
	TMap<TWeakObjectPtr<AActor>, TArray<FActiveGameplayEffectHandle>> ActiveInfiniteEffectHandles;

private:
	FTimerHandle CleanupTimerHandle;

	// Remove invalid actor entries from the map
	void CleanupInvalidHandles();
};