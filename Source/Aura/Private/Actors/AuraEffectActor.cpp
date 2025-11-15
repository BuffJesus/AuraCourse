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
			if (!IsValid(Actor))
			{
				continue;
			}

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
			if (!IsValid(TargetASC))
			{
				continue;
			}

			for (const FActiveGameplayEffectHandle& Handle : Pair.Value)
			{
				if (Handle.IsValid())
				{
					TargetASC->RemoveActiveGameplayEffect(Handle, 1);
				}
			}
		}
		ActiveInfiniteEffectHandles.Empty();
	}

	Super::EndPlay(EndPlayReason);
}

FActiveGameplayEffectHandle AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor,
	const TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!HasAuthority() || !IsValid(TargetActor) || !EffectClass)
	{
		return FActiveGameplayEffectHandle();
	}

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC))
	{
		return FActiveGameplayEffectHandle();
	}

	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle =
		TargetASC->MakeOutgoingSpec(EffectClass, ActorLevel, EffectContextHandle);
	if (!EffectSpecHandle.IsValid())
	{
		return FActiveGameplayEffectHandle();
	}

	return TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (!HasAuthority() || !IsValid(TargetActor) || TargetActor == this)
	{
		return;
	}

	// Instant effects on begin overlap
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : InstantGameplayEffects)
		{
			if (EffectClass)
			{
				ApplyEffectToTarget(TargetActor, EffectClass);
			}
		}
	}

	// Duration effects on begin overlap
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : DurationGameplayEffects)
		{
			if (EffectClass)
			{
				ApplyEffectToTarget(TargetActor, EffectClass);
			}
		}
	}

	// Periodic effects on begin overlap
	if (PeriodicEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : PeriodicGameplayEffects)
		{
			if (EffectClass)
			{
				ApplyEffectToTarget(TargetActor, EffectClass);
			}
		}
	}

	// Infinite effects on begin overlap – track handles so we can remove them later
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		TArray<FActiveGameplayEffectHandle>& Handles = ActiveInfiniteEffectHandles.FindOrAdd(TargetActor);
		for (const TSubclassOf<UGameplayEffect>& EffectClass : InfiniteGameplayEffects)
		{
			if (!EffectClass)
			{
				continue;
			}

			const FActiveGameplayEffectHandle Handle = ApplyEffectToTarget(TargetActor, EffectClass);
			if (Handle.IsValid())
			{
				Handles.Add(Handle);
			}
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (!HasAuthority() || !IsValid(TargetActor) || TargetActor == this)
	{
		return;
	}

	// Apply instant effects on end overlap if configured that way
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : InstantGameplayEffects)
		{
			if (EffectClass)
			{
				ApplyEffectToTarget(TargetActor, EffectClass);
			}
		}
	}

	// Apply duration effects on end overlap if configured that way
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : DurationGameplayEffects)
		{
			if (EffectClass)
			{
				ApplyEffectToTarget(TargetActor, EffectClass);
			}
		}
	}

	// Apply periodic effects on end overlap if configured that way
	if (PeriodicEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		for (const TSubclassOf<UGameplayEffect>& EffectClass : PeriodicGameplayEffects)
		{
			if (EffectClass)
			{
				ApplyEffectToTarget(TargetActor, EffectClass);
			}
		}
	}

	// Remove infinite effects if configured
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC))
		{
			return;
		}

		if (TArray<FActiveGameplayEffectHandle>* HandlesPtr = ActiveInfiniteEffectHandles.Find(TargetActor))
		{
			for (const FActiveGameplayEffectHandle& Handle : *HandlesPtr)
			{
				if (Handle.IsValid())
				{
					TargetASC->RemoveActiveGameplayEffect(Handle, 1);
				}
			}

			ActiveInfiniteEffectHandles.Remove(TargetActor);
		}

		if (bDestroyOnEffectRemoval)
		{
			Destroy();
		}
	}
}

void AAuraEffectActor::CleanupInvalidHandles()
{
	if (!HasAuthority())
	{
		return;
	}

	for (auto It = ActiveInfiniteEffectHandles.CreateIterator(); It; ++It)
	{
		if (!It->Key.IsValid())
		{
			It.RemoveCurrent();
		}
	}
}