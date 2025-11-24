// Not Sure Yet


#include "Characters/AuraBaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

FVector AAuraBaseCharacter::GetCombatSocketLocation() const
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraBaseCharacter::InitializeAbilityActorInfo()
{
	// override in children
}

void AAuraBaseCharacter::InitializeDefaultAttributes() const
{
	// Apply Primary Attributes first
	for (const TSubclassOf<UGameplayEffect>& Effect : DefaultPrimaryAttributes)
	{
		if (Effect) { ApplyDefaultGameplayEffect(Effect); }
	}

	// Apply Secondary Attributes second (these depend on Primary)
	for (const TSubclassOf<UGameplayEffect>& Effect : DefaultSecondaryAttributes)
	{
		if (Effect) { ApplyDefaultGameplayEffect(Effect); }
	}

	// Apply Vital Attributes last (these depend on Secondary like MaxHealth/MaxMana)
	for (const TSubclassOf<UGameplayEffect>& Effect : DefaultVitalAttributes)
	{
		if (Effect) { ApplyDefaultGameplayEffect(Effect); }
	}
}

void AAuraBaseCharacter::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
	if (!HasAuthority()) { return; }

	AuraASC->AddCharacterAbilities(StartupAbilities);
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
