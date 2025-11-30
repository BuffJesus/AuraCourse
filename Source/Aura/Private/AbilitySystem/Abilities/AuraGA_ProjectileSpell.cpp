// Not Sure Yet

#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/AuraProjectile.h"
#include "Tags/AuraTags.h"

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
											   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
											   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
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

        AAuraProjectile* Projectile { GetWorld()->SpawnActorDeferred<AAuraProjectile>(
                ProjectileClass,
                SpawnTransform,
                GetOwningActorFromActorInfo(),
                Cast<APawn>(GetOwningActorFromActorInfo()),
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

        const FGameplayEffectSpecHandle SpecHandle { MakeDamageEffectSpecHandle(EffectContextHandle) };
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
