// Not Sure Yet

#include "AbilitySystem/Cues/AuraGCN_DamageText.h"
#include "UI/Widgets/AuraDamageTextComponent.h"
#include "GameFramework/Character.h"

bool UAuraGCN_DamageText::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (!DamageTextComponentClass)
	{
		return false;
	}
	
	// Use the Location parameter to find the actual damage recipient
	ACharacter* TargetCharacter { nullptr };
    
	// If Location is set, trace to find the character at that location
	if (!Parameters.Location.IsZero())
	{
		TargetCharacter = Cast<ACharacter>(Target);
	}
	else
	{
		TargetCharacter = Cast<ACharacter>(Target);
	}

	if (!TargetCharacter)
	{
		return false;
	}

	const float Damage { Parameters.RawMagnitude };

	UAuraDamageTextComponent* DamageTextComponent { NewObject<UAuraDamageTextComponent>(TargetCharacter, DamageTextComponentClass) };
	DamageTextComponent->RegisterComponent();
	DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DamageTextComponent->SetDamageText(Damage);

	return true;
}