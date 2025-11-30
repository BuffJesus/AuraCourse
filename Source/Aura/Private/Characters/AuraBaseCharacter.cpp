
#include "Characters/AuraBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AuraCollisionChannels.h"
#include "MotionWarpingComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Tags/AuraTags.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	
	// Ensure timeline component is valid and properly initialized
	if (!IsValid(DissolveTimeline))
	{
		UE_LOG(LogTemp, Warning, TEXT("DissolveTimeline is not valid on %s"), *GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] BeginPlay: Timeline setup starting. DissolveCurve=%s, GlowCurve=%s"), 
		*GetName(), 
		DissolveCurve ? *DissolveCurve->GetName() : TEXT("NULL"),
		GlowCurve ? *GlowCurve->GetName() : TEXT("NULL"));

	// Lambda to reduce duplication in timeline curve binding
	auto BindCurveToTimeline = [this](UCurveFloat* Curve, const FName& CallbackName)
	{
		if (Curve)
		{
			FOnTimelineFloat Callback;
			Callback.BindUFunction(this, CallbackName);
			DissolveTimeline->AddInterpFloat(Curve, Callback);
			UE_LOG(LogTemp, Warning, TEXT("[%s] Bound curve %s to callback %s"), 
				*GetName(), *Curve->GetName(), *CallbackName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Curve for callback %s is not assigned"), 
				*GetName(), *CallbackName.ToString());
		}
	};
	
	BindCurveToTimeline(DissolveCurve, FName("UpdateDissolveMaterial"));
	BindCurveToTimeline(GlowCurve, FName("UpdateGlowMaterial"));
	
    // Configure timeline settings
    DissolveTimeline->SetLooping(false);
    DissolveTimeline->SetIgnoreTimeDilation(false);

    // Ensure the timeline component can tick even if the actor itself doesn't
    // This allows the dissolve/glow curves to drive parameter updates every frame
    DissolveTimeline->PrimaryComponentTick.bCanEverTick = true;
    DissolveTimeline->SetComponentTickEnabled(true);
    // Make sure the component is registered so it will receive ticks
    if (!DissolveTimeline->IsRegistered())
    {
        DissolveTimeline->RegisterComponent();
    }

    // Bind finished callback so we can stop ticking after the dissolve finishes
    {
        FOnTimelineEvent FinishedCallback;
        FinishedCallback.BindUFunction(this, FName("OnDissolveFinished"));
        DissolveTimeline->SetTimelineFinishedFunc(FinishedCallback);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("[%s] Timeline configured."), *GetName());
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
    // Server-side: mark dead, block/stop abilities, stop animations & AI
    bDead = true;
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->AddLooseGameplayTag(Aura::Ability::State::Dead);
        // Cancel all running abilities on death
        AbilitySystemComponent->CancelAbilities();
    }

    if (UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
    {
        AnimInst->StopAllMontages(0.25f);
    }

    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately();
    }

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
	UE_LOG(LogTemp, Warning, TEXT("[%s] Dissolve() called"), *GetName());

	if (IsValid(DissolveMaterialInstance))
	{
		DynamicDissolveMaterial = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterial);
		UE_LOG(LogTemp, Warning, TEXT("[%s] Created dynamic dissolve material"), *GetName());
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		DynamicWeaponDissolveMaterial = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicWeaponDissolveMaterial);
		UE_LOG(LogTemp, Warning, TEXT("[%s] Created dynamic weapon dissolve material"), *GetName());
	}
	
	if (IsValid(DissolveTimeline))
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Timeline is valid. IsActive=%d"), 
			*GetName(), DissolveTimeline->IsActive());
		
		// Enable actor ticking so the timeline can update
		SetActorTickEnabled(true);
		UE_LOG(LogTemp, Warning, TEXT("[%s] Actor ticking enabled"), *GetName());
		
		// Activate and play the timeline
		if (!DissolveTimeline->IsActive())
		{
			DissolveTimeline->Activate(true);
			UE_LOG(LogTemp, Warning, TEXT("[%s] Timeline activated"), *GetName());
		}
		
		DissolveTimeline->PlayFromStart();
		UE_LOG(LogTemp, Warning, TEXT("[%s] Timeline playing from start"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DissolveTimeline is invalid on %s - cannot play dissolve effect"), *GetName());
	}
}

void AAuraBaseCharacter::UpdateDissolveMaterial(float DissolveValue)
{
    if (IsValid(DynamicDissolveMaterial))
    {
        DynamicDissolveMaterial->SetScalarParameterValue(FName("Dissolve"), DissolveValue);
    }
    if (IsValid(DynamicWeaponDissolveMaterial))
    {
        DynamicWeaponDissolveMaterial->SetScalarParameterValue(FName("Dissolve"), DissolveValue);
    }
}

void AAuraBaseCharacter::UpdateGlowMaterial(float GlowValue)
{
    if (IsValid(DynamicDissolveMaterial))
    {
        DynamicDissolveMaterial->SetScalarParameterValue(FName("Glow"), GlowValue);
    }
    if (IsValid(DynamicWeaponDissolveMaterial))
    {
        DynamicWeaponDissolveMaterial->SetScalarParameterValue(FName("Glow"), GlowValue);
    }
}

void AAuraBaseCharacter::OnDissolveFinished()
{
    // Stop ticking once the effect is done
    if (DissolveTimeline)
    {
        DissolveTimeline->SetComponentTickEnabled(false);
        DissolveTimeline->Stop();
    }
    SetActorTickEnabled(false);
    UE_LOG(LogTemp, Log, TEXT("[%s] Dissolve finished."), *GetName());
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
            // Provide a yaw-only rotation with the actual target location. Using the real target location
            // prevents Facing from hitting zero-length edge cases that can flip orientation.
            const FQuat YawOnlyQuat = FRotator(0.f, LookRotation.Yaw, 0.f).Quaternion();
            const FTransform WarpTransform(YawOnlyQuat, Target);
            const FMotionWarpingTarget WarpTarget { FName("FacingTarget"), WarpTransform };
            MotionWarpingComponent->AddOrUpdateWarpTarget(WarpTarget);
        }
    }
}