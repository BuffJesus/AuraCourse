// Not Sure Yet

#include "AbilitySystem/Cues/AuraGCN_DamageText.h"

#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "UI/Widgets/AuraDamageTextComponent.h"
#include "GameFramework/Character.h"

bool UAuraGCN_DamageText::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (!DamageTextComponentClass) { return false; }
    
	ACharacter* TargetCharacter = Cast<ACharacter>(Target);
	if (!TargetCharacter) { return false; }

	const float Damage = Parameters.RawMagnitude;
    
	// Extract ALL hit type information from the custom effect context
	const bool bBlocked = UAuraAbilitySystemBPLibrary::IsBlockedHit(Parameters.EffectContext);
	const bool bCritical = UAuraAbilitySystemBPLibrary::IsCriticalHit(Parameters.EffectContext);
	const bool bNice = UAuraAbilitySystemBPLibrary::IsNiceHit(Parameters.EffectContext);
	const bool bDank = UAuraAbilitySystemBPLibrary::IsDankHit(Parameters.EffectContext);
	const bool bPi = UAuraAbilitySystemBPLibrary::IsPiHit(Parameters.EffectContext);

	UAuraDamageTextComponent* DamageTextComponent = NewObject<UAuraDamageTextComponent>(
		TargetCharacter, DamageTextComponentClass);
	DamageTextComponent->RegisterComponent();
	DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), 
		FAttachmentTransformRules::KeepRelativeTransform);
	DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DamageTextComponent->SetDamageText(Damage, bBlocked, bCritical, bNice, bDank, bPi);

	return true;
}