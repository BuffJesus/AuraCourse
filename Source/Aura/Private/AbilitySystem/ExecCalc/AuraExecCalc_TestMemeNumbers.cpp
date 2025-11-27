// Not Sure Yet

#include "AbilitySystem/ExecCalc/AuraExecCalc_TestMemeNumbers.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemBPLibrary.h"
#include "AbilitySystem/Attributes/AuraAttributeSet.h"
#include "Tags/AuraTags.h"

UAuraExecCalc_TestMemeNumbers::UAuraExecCalc_TestMemeNumbers()
{
	// No attributes to capture - we're just testing damage display
}

void UAuraExecCalc_TestMemeNumbers::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	
	// Get the base damage from SetByCaller (use Aura.Damage tag)
	float Damage = Spec.GetSetByCallerMagnitude(Aura::Damage::Damage, false, 0.f);
	
	// If no damage was set, default to 100 for testing
	if (Damage <= 0.f)
	{
		Damage = 100.f;
	}
	
	// Check for specific test damage values and set flags accordingly
	// These should be set via SetByCaller in your test ability
	
	// Test Case 1: Normal damage (e.g., 100)
	if (FMath::IsNearlyEqual(Damage, 100.f, 0.01f))
	{
		// Normal hit - no flags set
		UE_LOG(LogTemp, Warning, TEXT("TEST: Normal Hit - Damage: %.2f"), Damage);
	}
	
	// Test Case 2: Blocked hit (e.g., 50)
	else if (FMath::IsNearlyEqual(Damage, 50.f, 0.01f))
	{
		UAuraAbilitySystemBPLibrary::SetIsBlockedHit(EffectContextHandle, true);
		UE_LOG(LogTemp, Warning, TEXT("TEST: Blocked Hit - Damage: %.2f"), Damage);
	}
	
	// Test Case 3: Critical hit (e.g., 200)
	else if (FMath::IsNearlyEqual(Damage, 200.f, 0.01f))
	{
		UAuraAbilitySystemBPLibrary::SetIsCriticalHit(EffectContextHandle, true);
		UE_LOG(LogTemp, Warning, TEXT("TEST: Critical Hit - Damage: %.2f"), Damage);
	}
	
	// Test Case 4: Nice (69)
	else if (FMath::IsNearlyEqual(Damage, 69.f, 0.01f))
	{
		UAuraAbilitySystemBPLibrary::SetIsNiceHit(EffectContextHandle, true);
		UE_LOG(LogTemp, Warning, TEXT("TEST: Nice Hit - Damage: %.2f"), Damage);
	}
	
	// Test Case 5: Dank (420)
	else if (FMath::IsNearlyEqual(Damage, 420.f, 0.01f))
	{
		UAuraAbilitySystemBPLibrary::SetIsDankHit(EffectContextHandle, true);
		UE_LOG(LogTemp, Warning, TEXT("TEST: Dank Hit - Damage: %.2f"), Damage);
	}
	
	// Test Case 6: Pi (~3.14)
	else if (FMath::IsNearlyEqual(Damage, 3.14f, 0.01f) || FMath::IsNearlyEqual(Damage, PI, 0.01f))
	{
		UAuraAbilitySystemBPLibrary::SetIsPiHit(EffectContextHandle, true);
		UE_LOG(LogTemp, Warning, TEXT("TEST: Pi Hit - Damage: %.2f"), Damage);
	}
	
	// Test Case 7: Critical Block (e.g., 150)
	else if (FMath::IsNearlyEqual(Damage, 150.f, 0.01f))
	{
		UAuraAbilitySystemBPLibrary::SetIsBlockedHit(EffectContextHandle, true);
		UAuraAbilitySystemBPLibrary::SetIsCriticalHit(EffectContextHandle, true);
		UE_LOG(LogTemp, Warning, TEXT("TEST: Critical Blocked Hit - Damage: %.2f"), Damage);
	}
	
	// Test Case 8: Random damage for general testing
	else
	{
		// Randomly apply flags for variety
		const bool bRandomBlock = FMath::RandBool();
		const bool bRandomCrit = FMath::RandBool();
		
		if (bRandomBlock)
		{
			UAuraAbilitySystemBPLibrary::SetIsBlockedHit(EffectContextHandle, true);
		}
		if (bRandomCrit)
		{
			UAuraAbilitySystemBPLibrary::SetIsCriticalHit(EffectContextHandle, true);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("TEST: Random Hit - Damage: %.2f, Blocked: %s, Crit: %s"), 
			Damage, bRandomBlock ? TEXT("Yes") : TEXT("No"), bRandomCrit ? TEXT("Yes") : TEXT("No"));
	}
	
	// Apply the damage
	const FGameplayModifierEvaluatedData EvaluatedData(
		UAuraAttributeSet::GetIncomingDamageAttribute(), 
		EGameplayModOp::Additive, 
		Damage
	);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}