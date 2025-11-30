// Not Sure Yet

#include "Characters/AuraBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AuraCollisionChannels.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Tags/AuraTags.h"

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
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);

	for (const auto& [Channel, Response] : MeshCollisionResponses)
	{ GetMesh()->SetCollisionResponseToChannel(Channel, Response); }

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), WeaponSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(true);
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
	bDead = true;
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
	if (!IsValid(Weapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon not valid, using actor location"));
		return GetActorLocation();
	}
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

FVector AAuraBaseCharacter::GetCombatSocketLocationByTag(const FGameplayTag& SocketTag) const
{
	// First check the TaggedCombatSockets map for custom mappings
	if (SocketTag.IsValid())
	{
		if (const FName* SocketName = TaggedCombatSockets.Find(SocketTag))
		{
			// Check weapon first, then character mesh
			if (Weapon && Weapon->DoesSocketExist(*SocketName))
			{
				return Weapon->GetSocketLocation(*SocketName);
			}
			if (GetMesh()->DoesSocketExist(*SocketName))
			{
				return GetMesh()->GetSocketLocation(*SocketName);
			}
		}

		// Handle standard combat socket tags using built-in socket names
		if (SocketTag.MatchesTagExact(Aura::CombatSocket::Weapon) && IsValid(Weapon))
		{
			return Weapon->GetSocketLocation(WeaponTipSocketName);
		}
		if (SocketTag.MatchesTagExact(Aura::CombatSocket::LeftHand))
		{
			return GetMesh()->GetSocketLocation(LeftHandSocketName);
		}
		if (SocketTag.MatchesTagExact(Aura::CombatSocket::RightHand))
		{
			return GetMesh()->GetSocketLocation(RightHandSocketName);
		}
	}

	// Fallback to default weapon socket
	return GetCombatSocketLocation();
}

UAnimMontage* AAuraBaseCharacter::GetHitReactMontage_Implementation() const
{
	return HitReactMontage;
}

UAnimMontage* AAuraBaseCharacter::GetAttackMontage_Implementation() const
{
	// Use the attack montage array if it has entries
	if (!AttackMontages.IsEmpty())
	{
		const int32 MontageIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
		return AttackMontages[MontageIndex].Montage;
	}
	return nullptr;
}

bool AAuraBaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraBaseCharacter::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraBaseCharacter::GetAttackMontages_Implementation() const
{
	return AttackMontages;
}

void AAuraBaseCharacter::InitializeAbilityActorInfo()
{
	// Override in children
}

void AAuraBaseCharacter::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemBPLibrary::InitializeDefaultAttributes(
		this,
		GetCharacterClass(),
		static_cast<float>(GetCharacterLevel()),
		AbilitySystemComponent);
}

void AAuraBaseCharacter::AddCharacterAbilities()
{
	if (!HasAuthority()) { return; }
	
	UAuraAbilitySystemBPLibrary::GiveStartupAbilities(this, AbilitySystemComponent, GetCharacterClass());
}

void AAuraBaseCharacter::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);
	}
	
	if (DissolveTimeline)
	{
		DissolveTimeline->PlayFromStart();
	}
}

void AAuraBaseCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	// Override or bind in Blueprint
}

void AAuraBaseCharacter::UpdateGlowMaterial(float GlowValue)
{
	// Override or bind in Blueprint
}

void AAuraBaseCharacter::UpdateFacingTarget_Implementation(const FVector& Target)
{
	const FVector ActorLocation { GetActorLocation() };
	FVector DirectionToTarget { Target - ActorLocation };
	DirectionToTarget.Z = 0.f;

	if (!DirectionToTarget.IsNearlyZero())
	{
		const FRotator LookRotation { DirectionToTarget.Rotation() };
		SetActorRotation(FRotator(0.f, LookRotation.Yaw, 0.f));

		if (IsValid(MotionWarpingComponent))
		{
			const FTransform WarpTransform { LookRotation, Target };
			const FMotionWarpingTarget WarpTarget { FName("FacingTarget"), WarpTransform };
			MotionWarpingComponent->AddOrUpdateWarpTarget(WarpTarget);
		}
	}
}