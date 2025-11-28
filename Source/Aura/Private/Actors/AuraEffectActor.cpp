// Not Sure Yet

#include "Actors/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	// Start periodic cleanup of invalid actor references (server only)
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(CleanupTimerHandle, this, 
			&AAuraEffectActor::CleanupInvalidHandles, 10.0f, true);
	}
}

void AAuraEffectActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Remove all tracked effects when this actor is destroyed
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(CleanupTimerHandle);

		for (const TPair<TWeakObjectPtr<AActor>, TArray<FActiveGameplayEffectHandle>>& Pair : ActiveInfiniteEffectHandles)
		{
			AActor* Actor = Pair.Key.Get();
			if (!IsValid(Actor)) { continue; }

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
			if (!IsValid(TargetASC)) { continue; }

			for (const FActiveGameplayEffectHandle& Handle : Pair.Value)
			{
				if (Handle.IsValid()) { TargetASC->RemoveActiveGameplayEffect(Handle, 1); }
			}
		}
		ActiveInfiniteEffectHandles.Empty();
	}

	Super::EndPlay(EndPlayReason);
}

FActiveGameplayEffectHandle AAuraEffectActor::ApplyEffectToTarget(
	AActor* TargetActor,
	const TSubclassOf<UGameplayEffect> EffectClass,
	const TArray<FSetByCallerMagnitude>& SetByCallerMagnitudes)
{
	if (!HasAuthority() || !IsValid(TargetActor) || !EffectClass) { return FActiveGameplayEffectHandle(); }

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC)) { return FActiveGameplayEffectHandle(); }

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle =
		TargetASC->MakeOutgoingSpec(EffectClass, ActorLevel, EffectContextHandle);
	if (!SpecHandle.IsValid()) { return FActiveGameplayEffectHandle(); }

	// Apply SetByCaller magnitudes if provided
	for (const FSetByCallerMagnitude& SetByCaller : SetByCallerMagnitudes)
	{
		if (SetByCaller.DataTag.IsValid()) { SpecHandle.Data->SetSetByCallerMagnitude(SetByCaller.DataTag, SetByCaller.Magnitude); }
	}

	return TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (!HasAuthority() || !IsValid(TargetActor) || TargetActor == this) { return; }

	// Lambda to apply a collection of effects based on policy
	auto ApplyEffectsOnPolicy = [this, TargetActor](
		const EEffectApplicationPolicy Policy,
		const TArray<TSubclassOf<UGameplayEffect>>& Effects,
		const TArray<FSetByCallerMagnitude>& Magnitudes)
	{
		if (Policy == EEffectApplicationPolicy::ApplyOnOverlap)
		{
			for (const TSubclassOf<UGameplayEffect>& EffectClass : Effects)
			{
				if (EffectClass)
				{
					ApplyEffectToTarget(TargetActor, EffectClass, Magnitudes);
				}
			}
		}
	};

	// Apply all effect types using the lambda
	ApplyEffectsOnPolicy(InstantEffectApplicationPolicy, InstantGameplayEffects, InstantSetByCallerMagnitudes);
	ApplyEffectsOnPolicy(DurationEffectApplicationPolicy, DurationGameplayEffects, DurationSetByCallerMagnitudes);
	ApplyEffectsOnPolicy(PeriodicEffectApplicationPolicy, PeriodicGameplayEffects, PeriodicSetByCallerMagnitudes);

	// Handle infinite effects (need to track handles)
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		TArray<FActiveGameplayEffectHandle>& Handles = ActiveInfiniteEffectHandles.FindOrAdd(TargetActor);
		for (const TSubclassOf<UGameplayEffect>& EffectClass : InfiniteGameplayEffects)
		{
			if (!EffectClass) { continue; }

			const FActiveGameplayEffectHandle Handle = ApplyEffectToTarget(TargetActor, EffectClass, InfiniteSetByCallerMagnitudes);
			if (Handle.IsValid())
			{
				Handles.Add(Handle);
			}
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (!HasAuthority() || !IsValid(TargetActor) || TargetActor == this) { return; }

	// Lambda to apply a collection of effects based on policy
	auto ApplyEffectsOnPolicy = [this, TargetActor](
		const EEffectApplicationPolicy Policy,
		const TArray<TSubclassOf<UGameplayEffect>>& Effects,
		const TArray<FSetByCallerMagnitude>& Magnitudes)
	{
		if (Policy == EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			for (const TSubclassOf<UGameplayEffect>& EffectClass : Effects)
			{
				if (EffectClass)
				{
					ApplyEffectToTarget(TargetActor, EffectClass, Magnitudes);
				}
			}
		}
	};

	// Apply all effect types using the lambda
	ApplyEffectsOnPolicy(InstantEffectApplicationPolicy, InstantGameplayEffects, InstantSetByCallerMagnitudes);
	ApplyEffectsOnPolicy(DurationEffectApplicationPolicy, DurationGameplayEffects, DurationSetByCallerMagnitudes);
	ApplyEffectsOnPolicy(PeriodicEffectApplicationPolicy, PeriodicGameplayEffects, PeriodicSetByCallerMagnitudes);

	// Handle infinite effect removal
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) { return; }

		if (TArray<FActiveGameplayEffectHandle>* HandlesPtr = ActiveInfiniteEffectHandles.Find(TargetActor))
		{
			for (const FActiveGameplayEffectHandle& Handle : *HandlesPtr)
			{
				if (Handle.IsValid()) { TargetASC->RemoveActiveGameplayEffect(Handle, 1); }
			}

			ActiveInfiniteEffectHandles.Remove(TargetActor);
		}

		if (bDestroyOnEffectRemoval) { Destroy(); }
	}
}

void AAuraEffectActor::CleanupInvalidHandles()
{
	if (!HasAuthority()) { return; }

	// FIXED: Clean up invalid entries
	for (auto It = ActiveInfiniteEffectHandles.CreateIterator(); It; ++It)
	{
		if (!It->Key.IsValid()) { It.RemoveCurrent(); }
	}
	
	// FIXED: Stop timer if nothing left to clean - prevents unnecessary timer ticks
	if (ActiveInfiniteEffectHandles.Num() == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CleanupTimerHandle);
		UE_LOG(LogTemp, Verbose, TEXT("AuraEffectActor: Cleanup timer stopped (no active effects)"));
	}
}