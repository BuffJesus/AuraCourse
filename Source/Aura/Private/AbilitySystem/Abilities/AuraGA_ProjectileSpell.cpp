// Not Sure Yet

#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/AuraProjectile.h"
#include "Animation/AnimInstance.h"
#include "Interaction/AuraCombatInterface.h"
#include "Tags/AuraTags.h"

UAuraGA_ProjectileSpell::UAuraGA_ProjectileSpell()
{
        InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                                                           const FGameplayEventData* TriggerEventData)
{
Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

AActor* AvatarActor = GetAvatarActorFromActorInfo();
if (!AvatarActor)
{
EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
return;
}

if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
{
EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
return;
}

UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
if (!ASC)
{
UE_LOG(LogTemp, Error, TEXT("%s: No ASC on AvatarActor when activating projectile spell"), *GetName());
EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
return;
}

if (const IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(AvatarActor))
{
if (UAnimMontage* MontageToPlay = CombatInterface->Execute_GetAttackMontage(AvatarActor))
{
PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
this,
NAME_None,
MontageToPlay,
1.0f
);

if (PlayMontageTask)
{
PlayMontageTask->OnCompleted.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCompleted);
PlayMontageTask->OnInterrupted.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCancelled);
PlayMontageTask->OnCancelled.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCancelled);
PlayMontageTask->OnBlendOut.AddDynamic(this, &UAuraGA_ProjectileSpell::OnMontageCompleted);

PlayMontageTask->ReadyForActivation();
return;
}

UE_LOG(LogTemp, Warning, TEXT("%s: Montage task failed, falling back to direct montage play"), *GetName());

FallbackAnimInstance = ActorInfo->GetAnimInstance();
if (FallbackAnimInstance.IsValid())
{
FallbackMontageDelegate.Unbind();
FallbackMontageDelegate.BindUObject(this, &UAuraGA_ProjectileSpell::OnFallbackMontageEnded);
FallbackAnimInstance->Montage_SetEndDelegate(FallbackMontageDelegate, MontageToPlay);

if (FallbackAnimInstance->Montage_Play(MontageToPlay, 1.f) > 0.f)
{
return;
}
}

UE_LOG(LogTemp, Error, TEXT("%s: Unable to play attack montage"), *GetName());
EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
return;
}
}

// If no montage is available, end immediately to avoid getting stuck in an active state
EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UAuraGA_ProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
        const bool bIsServer { GetAvatarActorFromActorInfo()->HasAuthority() };
        if (!bIsServer) return;

        const UAbilitySystemComponent* SourceASC { UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()) };
        if (!IsValid(SourceASC))
        {
                UE_LOG(LogTemp, Error, TEXT("No AbilitySystemComponent found for %s when spawning projectile"), *GetName());
                return;
        }

        if (!ProjectileClass || !DamageEffectClass)
        {
                UE_LOG(LogTemp, Error, TEXT("ProjectileClass or DamageEffectClass not set on %s"), *GetName());
                return;
        }

        const FVector SocketLocation { GetCombatSocketLocation() };
        if (SocketLocation.IsNearlyZero())
        {
                UE_LOG(LogTemp, Error, TEXT("Invalid combat socket location on %s when spawning projectile"), *GetName());
                return;
        }

        const FRotator Rotation { (TargetLocation - SocketLocation).Rotation() };

        FTransform SpawnTransform(Rotation.Quaternion(), SocketLocation);

        AActor* AvatarActor { GetAvatarActorFromActorInfo() };

        AAuraProjectile* Projectile { GetWorld()->SpawnActorDeferred<AAuraProjectile>(
                ProjectileClass,
                SpawnTransform,
                AvatarActor,
                Cast<APawn>(AvatarActor),
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn) };

        FGameplayEffectContextHandle EffectContextHandle { SourceASC->MakeEffectContext() };
        EffectContextHandle.SetAbility(this);
        EffectContextHandle.AddSourceObject(Projectile);
        TArray<TWeakObjectPtr<AActor>> Actors;
        Actors.Add(Projectile);
        EffectContextHandle.AddActors(Actors);
        FHitResult HitResult;
        HitResult.Location = TargetLocation;
        EffectContextHandle.AddHitResult(HitResult);

        const FGameplayEffectSpecHandle SpecHandle { MakeDamageEffectSpecHandleWithContext(EffectContextHandle) };
        if (!SpecHandle.IsValid())
        {
                UE_LOG(LogTemp, Error, TEXT("Failed to create damage spec for %s"), *GetName());
                Projectile->Destroy();
                return;
        }

        Projectile->DamageEffectSpecHandle = SpecHandle;

        // Only override cue tags if ability has them configured
        // Otherwise, projectile keeps its Blueprint defaults
        if (ProjectileFlightCue.IsValid()) { Projectile->FlightCueTag = ProjectileFlightCue; }
        if (ProjectileImpactCue.IsValid()) { Projectile->ImpactCueTag = ProjectileImpactCue; }

        Projectile->FinishSpawning(SpawnTransform);
}

void UAuraGA_ProjectileSpell::OnMontageCompleted()
{
ClearFallbackMontageDelegate();
EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAuraGA_ProjectileSpell::OnMontageCancelled()
{
ClearFallbackMontageDelegate();
EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAuraGA_ProjectileSpell::OnFallbackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
ClearFallbackMontageDelegate();

if (bInterrupted)
{
OnMontageCancelled();
return;
}

OnMontageCompleted();
}

void UAuraGA_ProjectileSpell::ClearFallbackMontageDelegate()
{
if (FallbackAnimInstance.IsValid())
{
FallbackAnimInstance->Montage_SetEndDelegate(FOnMontageEnded(), nullptr);
}
}
