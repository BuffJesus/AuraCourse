// Not Sure Yet

#pragma once

/**
 * Shared macros for AttributeSet implementations
 * These macros reduce boilerplate when creating attributes with replication
 */

// Implements the OnRep function for an attribute
#define IMPLEMENT_ATTRIBUTE_ONREP(ClassName, AttributeName) \
void ClassName::OnRep_##AttributeName(const FGameplayAttributeData& Old##AttributeName) const \
{ \
GAMEPLAYATTRIBUTE_REPNOTIFY(ClassName, AttributeName, Old##AttributeName); \
}

// Registers an attribute for replication
#define REPLICATE_ATTRIBUTE(ClassName, AttributeName) \
DOREPLIFETIME_CONDITION_NOTIFY(ClassName, AttributeName, COND_None, REPNOTIFY_Always)