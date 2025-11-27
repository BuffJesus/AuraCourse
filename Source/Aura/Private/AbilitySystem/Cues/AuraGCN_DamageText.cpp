// Not Sure Yet

#include "AbilitySystem/Cues/AuraGCN_DamageText.h"
#include "UI/Widgets/AuraDamageTextComponent.h"
#include "GameFramework/Character.h"

bool UAuraGCN_DamageText::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (!DamageTextComponentClass) { return false; }
    
	ACharacter* TargetCharacter = Cast<ACharacter>(Target);
	if (!TargetCharacter) { return false; }

	const float Damage = Parameters.RawMagnitude;
    
	// Decode hit type from NormalizedMagnitude
	const float HitTypeEncoded = Parameters.NormalizedMagnitude;
	const bool bBlocked = (HitTypeEncoded == 1.f || HitTypeEncoded == 3.f);
	const bool bCritical = (HitTypeEncoded == 2.f || HitTypeEncoded == 3.f);

	UAuraDamageTextComponent* DamageTextComponent = NewObject<UAuraDamageTextComponent>(
		TargetCharacter, DamageTextComponentClass);
	DamageTextComponent->RegisterComponent();
	DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), 
		FAttachmentTransformRules::KeepRelativeTransform);
	DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DamageTextComponent->SetDamageText(Damage, bBlocked, bCritical);

	return true;
}