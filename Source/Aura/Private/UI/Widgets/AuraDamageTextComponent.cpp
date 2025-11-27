// Copyright Druid Mechanics

#include "UI/Widgets/AuraDamageTextComponent.h"

UAuraDamageTextComponent::UAuraDamageTextComponent()
{
	SetWidgetSpace(EWidgetSpace::Screen);
}

EAuraHitType UAuraDamageTextComponent::DetermineHitType(bool bBlocked, bool bCritical, bool bNice, bool bDank, bool bPi)
{
	// Priority order: Meme numbers > Critical/Block > Normal
	if (bNice) return EAuraHitType::Nice;
	if (bDank) return EAuraHitType::Dank;
	if (bPi) return EAuraHitType::Pi;
	
	// Handle critical and blocked
	if (bCritical && bBlocked) return EAuraHitType::CriticalBlocked;
	if (bCritical) return EAuraHitType::Critical;
	if (bBlocked) return EAuraHitType::Blocked;
	
	return EAuraHitType::Normal;
}

FText UAuraDamageTextComponent::GetHitTypeText(EAuraHitType HitType)
{
	switch (HitType)
	{
	case EAuraHitType::Normal:
		return FText::GetEmpty();
	case EAuraHitType::Blocked:
		return FText::FromString(TEXT("BLOCKED!"));
	case EAuraHitType::Critical:
		return FText::FromString(TEXT("CRITICAL!"));
	case EAuraHitType::Nice:
		return FText::FromString(TEXT("NICE!"));
	case EAuraHitType::Dank:
		return FText::FromString(TEXT("BLAZE IT!"));
	case EAuraHitType::Pi:
		return FText::FromString(TEXT("IRRATIONAL!"));
	case EAuraHitType::CriticalBlocked:
		return FText::FromString(TEXT("CRIT BLOCK!"));
	default:
		return FText::GetEmpty();
	}
}
