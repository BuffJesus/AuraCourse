// Not Sure Yet

#include "AbilitySystem/Abilities/AuraGA_ProjectileSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/AuraProjectile.h"
#include "Tags/AuraTags.h"
#include "Interaction/AuraCombatInterface.h"

void UAuraGA_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
											   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
											   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraGA_ProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	IAuraCombatInterface* CombatInterface = Cast<IAuraCombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		const FRotator Rotation = (TargetLocation - SocketLocation).Rotation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// Set up damage effect
		const UAbilitySystemComponent* SourceASC { UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()) };
		FGameplayEffectContextHandle EffectContextHandle { SourceASC->MakeEffectContext() };
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = TargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
		const FGameplayEffectSpecHandle SpecHandle { SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle) };
		
		const float ScaledDamage { Damage.GetValueAtLevel(GetAbilityLevel()) };
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Aura::Damage::Damage, ScaledDamage);
		Projectile->DamageEffectSpecHandle = SpecHandle;
		
		// Only override cue tags if ability has them configured
		// Otherwise, projectile keeps its Blueprint defaults
		if (ProjectileFlightCue.IsValid())
		{
			Projectile->FlightCueTag = ProjectileFlightCue;
		}
		
		if (ProjectileImpactCue.IsValid())
		{
			Projectile->ImpactCueTag = ProjectileImpactCue;
		}
		
		Projectile->FinishSpawning(SpawnTransform);
	}
}