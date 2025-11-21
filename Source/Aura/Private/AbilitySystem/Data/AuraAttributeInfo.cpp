// Not Sure Yet


#include "AbilitySystem/Data/AuraAttributeInfo.h"

FAttributeInfo UAuraAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FAttributeInfo& Info : AttributeInfo)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag)) { return Info; }
	}
	if (bLogNotFound) { UE_LOG(LogTemp,
		Warning,
		TEXT("Attribute Tag [%s] not found in AuraAttributeInfo [%s"),
		*AttributeTag.ToString(),*GetNameSafe(this)); }
	return FAttributeInfo();
}
