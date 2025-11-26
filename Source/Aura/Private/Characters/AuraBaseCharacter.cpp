// Not Sure Yet


#include "Characters/AuraBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AuraCollisionChannels.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "Components/CapsuleComponent.h"

namespace 
{
	constexpr TPair<ECollisionChannel, ECollisionResponse> MeshCollisionResponses[]
	{ {ECC_Camera, ECR_Ignore}, {ECC_Projectile, ECR_Overlap} };
}

AAuraBaseCharacter::AAuraBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>("DissolveTimeline");

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	for (const auto& [Channel, Response] : MeshCollisionResponses)
	{ GetMesh()->SetCollisionResponseToChannel(Channel, Response); }

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Lambda to reduce duplication in timeline curve binding
	auto BindCurveToTimeline = [this](UCurveFloat* Curve, const FName& CallbackName)
	{
		if (Curve)
		{
			FOnTimelineFloat Callback;
			Callback.BindUFunction(this, CallbackName);
			DissolveTimeline->AddInterpFloat(Curve, Callback);
		}
	};
	
	BindCurveToTimeline(DissolveCurve, FName("UpdateDissolveMaterial"));
	BindCurveToTimeline(GlowCurve, FName("UpdateGlowMaterial"));
	
	// Configure timeline settings if we have any curves
	if (DissolveCurve || GlowCurve)
	{
		DissolveTimeline->SetLooping(false);
		DissolveTimeline->SetIgnoreTimeDilation(false);
	}
}

void AAuraBaseCharacter::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
}


void AAuraBaseCharacter::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
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
	UAuraAbilitySystemBPLibrary::InitializeDefaultAttributes(this, GetCharacterClass(), 1.f, AbilitySystemComponent);
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

void AAuraBaseCharacter::Dissolve()
{
	DissolveMaterialInstances.Empty();

	// Lambda to apply dissolve material to a mesh component
	// Captures 'this' to add MIDs to the instance array
	auto ApplyDissolve = [this](const int32 MaterialIndex, UMaterialInstance* MaterialInstance, USkeletalMeshComponent* MeshComponent)
	{
		if (IsValid(MaterialInstance))
		{
			UMaterialInstanceDynamic* MIDynamic = UMaterialInstanceDynamic::Create(MaterialInstance, this);
			MeshComponent->SetMaterial(MaterialIndex, MIDynamic);
			DissolveMaterialInstances.Add(MIDynamic);
		}
	};

	ApplyDissolve(0, DissolveMaterialInstance, GetMesh());
	ApplyDissolve(0, WeaponDissolveMaterialInstance, Weapon);

	if (DissolveCurve || GlowCurve) { DissolveTimeline->PlayFromStart(); }
}

void AAuraBaseCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	UpdateMaterialParameter(DissolveParameterName, DissolveValue);
}

void AAuraBaseCharacter::UpdateGlowMaterial(float GlowValue)
{
	UpdateMaterialParameter(GlowParameterName, GlowValue);
}

void AAuraBaseCharacter::UpdateMaterialParameter(const FName& ParameterName, float Value)
{
	for (auto& MID : DissolveMaterialInstances)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(ParameterName, Value);
		}
	}
}

void AAuraBaseCharacter::ApplyGameplayEffectClassToSelf(const TSubclassOf<UGameplayEffect> EffectClass, const float Level) const
{
	check(IsValid(AbilitySystemComponent));
	check(EffectClass);
	FGameplayEffectContextHandle EffectContext { AbilitySystemComponent->MakeEffectContext() };
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle { AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level, EffectContext) };
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
}