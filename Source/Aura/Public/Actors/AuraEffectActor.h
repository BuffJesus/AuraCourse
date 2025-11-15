// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
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

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;

	// Return the handle so we can later remove infinite effects safely
	UFUNCTION(BlueprintCallable)
	FActiveGameplayEffectHandle ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClass) const;

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	bool bDestroyOnEffectRemoval = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> InstantGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> DurationGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> PeriodicGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectApplicationPolicy PeriodicEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Aura|Effects")
	TArray<TSubclassOf<UGameplayEffect>> InfiniteGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	// Track infinite effect handles per overlapping actor
	TMap<TWeakObjectPtr<AActor>, TArray<FActiveGameplayEffectHandle>> ActiveInfiniteEffectHandles;

};
