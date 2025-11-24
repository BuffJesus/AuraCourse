
// Not Sure Yet


#include "Characters/AuraBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AuraCollisionChannels.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	const TPair<ECollisionChannel, ECollisionResponse> MeshCollisionResponses[] 
	{ {ECC_Camera, ECR_Ignore}, {ECC_Projectile, ECR_Overlap} };
	
	for (const auto& [Channel, Response] : MeshCollisionResponses)
	{
		GetMesh()->SetCollisionResponseToChannel(Channel, Response);
	}

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
	// No cast needed! Already have UAuraAbilitySystemComponent*
	if (!HasAuthority()) { return; }

	AbilitySystemComponent->AddCharacterAbilities(StartupAbilities);
}

void AAuraBaseCharacter::UpdateFacingTarget_Implementation(const FVector& Target)
{
	if (MotionWarpingComponent)
	{
		MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(FacingTargetWarpName, Target);
	}
}

void AAuraBaseCharacter::ApplyDefaultGameplayEffect(const TSubclassOf<UGameplayEffect> EffectClass, const float Level) const
{
	check(IsValid(AbilitySystemComponent));
	check(EffectClass);
	FGameplayEffectContextHandle EffectContext { AbilitySystemComponent->MakeEffectContext() };
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle { AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level, EffectContext) };
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
}
