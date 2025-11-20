// Not Sure Yet


#include "Characters/AuraBaseCharacter.h"

#include "AbilitySystemComponent.h"

AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraBaseCharacter::InitializeAbilityActorInfo()
{
	
}

void AAuraBaseCharacter::InitializeDefaultAttributes() const
{
	// Apply Primary Attributes first
	for (const TSubclassOf<UGameplayEffect>& Effect : DefaultPrimaryAttributes)
	{
		if (Effect)
		{
			ApplyDefaultGameplayEffect(Effect, 1.f);
		}
	}

	// Apply Secondary Attributes second (these depend on Primary)
	for (const TSubclassOf<UGameplayEffect>& Effect : DefaultSecondaryAttributes)
	{
		if (Effect)
		{
			ApplyDefaultGameplayEffect(Effect, 1.f);
		}
	}

	// Apply Vital Attributes last (these depend on Secondary like MaxHealth/MaxMana)
	for (const TSubclassOf<UGameplayEffect>& Effect : DefaultVitalAttributes)
	{
		if (Effect)
		{
			ApplyDefaultGameplayEffect(Effect, 1.f);
		}
	}
}

void AAuraBaseCharacter::ApplyDefaultGameplayEffect(const TSubclassOf<UGameplayEffect> EffectClass, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(EffectClass);
	FGameplayEffectContextHandle EffectContext { GetAbilitySystemComponent()->MakeEffectContext() };
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle { GetAbilitySystemComponent()->MakeOutgoingSpec(EffectClass, Level, EffectContext) };
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}