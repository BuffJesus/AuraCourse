// Not Sure Yet

#include "AbilitySystem/Cues/AuraGCN_DamageText.h"
#include "UI/Widgets/AuraDamageTextComponent.h"
#include "GameFramework/Character.h"

bool UAuraGCN_DamageText::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (!IsValid(Target) || !DamageTextComponentClass)
	{
		return false;
	}

	ACharacter* TargetCharacter { Cast<ACharacter>(Target) };
	if (!TargetCharacter)
	{
		return false;
	}

	// Extract damage from the magnitude parameter
	const float Damage { Parameters.RawMagnitude };

	// Create and configure the damage text component
	UAuraDamageTextComponent* DamageTextComponent { NewObject<UAuraDamageTextComponent>(TargetCharacter, DamageTextComponentClass) };
	DamageTextComponent->RegisterComponent();
	DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DamageTextComponent->SetDamageText(Damage);

	return true;
}