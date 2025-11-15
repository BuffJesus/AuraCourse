// Not Sure Yet


#include "Actors/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

FActiveGameplayEffectHandle AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor,
	const TSubclassOf<UGameplayEffect> EffectClass) const
{
	// Authority check - only apply effects on server
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
		TargetASC->MakeOutgoingSpec(EffectClass, 1.f, EffectContextHandle);
	if (!EffectSpecHandle.IsValid())
	{
		return FActiveGameplayEffectHandle();
	}

	return TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	// Authority check - only run on server
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
	// Authority check - only run on server
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

		TArray<FActiveGameplayEffectHandle>* HandlesPtr = ActiveInfiniteEffectHandles.Find(TargetActor);
		if (HandlesPtr)
		{
			for (const FActiveGameplayEffectHandle& Handle : *HandlesPtr)
			{
				if (Handle.IsValid())
				{
					// Remove entire effect (or adjust stack count as needed)
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
