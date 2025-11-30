// Not Sure Yet

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AuraCollisionChannels.h"
#include "GameplayTagAssetInterface.h"
#include "Interaction/AuraCombatInterface.h"
#include "AbilitySystem/Data/AuraCharacterClassInfo.h"
#include "Components/TimelineComponent.h"
#include "AuraBaseCharacter.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAbilitySystemComponent;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;
class UMotionWarpingComponent;

UCLASS(Abstract)
class AURA_API AAuraBaseCharacter : public ACharacter, public IAbilitySystemInterface, public IAuraCombatInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AAuraBaseCharacter();

	virtual void Die() override;
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	// Interface requirement - returns base type
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// Typed getter - returns UAuraAbilitySystemComponent (no cast needed!)
	FORCEINLINE UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	// Typed getter - returns UAuraAttributeSet (no cast needed!)
	FORCEINLINE UAuraAttributeSet* GetAuraAttributeSet() const { return AttributeSet; }

	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName WeaponSocketName { "WeaponHandSocket" };
	
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName WeaponTipSocketName { "WeaponTipSocket" };

	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName LeftHandSocketName { "LeftHandSocket" };

	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	FName RightHandSocketName { "RightHandSocket" };

	virtual FVector GetCombatSocketLocation() const override;
	virtual FVector GetCombatSocketLocationByTag(const FGameplayTag& SocketTag) const override;

	/** Optional mapping of Gameplay Tags to specific combat sockets (e.g., left/right hands) */
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	TMap<FGameplayTag, FName> TaggedCombatSockets;

	/** Array of attack montages with their associated socket tags for multi-attack characters */
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	TArray<FTaggedMontage> AttackMontages;
	
	UFUNCTION(BlueprintCallable, Category = "Aura|Combat")
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aura|Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aura|Tags")
	FGameplayTagContainer CharacterTags;
	
    FORCEINLINE virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override
    {
            TagContainer = CharacterTags;
    };

	virtual ECharacterClass GetCharacterClass() const { return ECharacterClass::DefaultClass; }

	/** Combat Interface Implementations */
	virtual UAnimMontage* GetHitReactMontage_Implementation() const override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() const override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "Aura|Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	bool bDead = false;

	// Store as typed pointer - no casting needed when accessing!
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, Category = "Aura|Combat")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	virtual void InitializeAbilityActorInfo();
	virtual void InitializeDefaultAttributes() const;
	void AddCharacterAbilities();

	/* Dissolve Effects */
	UPROPERTY(VisibleAnywhere, Category = "Aura|Effects")
	TObjectPtr<UTimelineComponent> DissolveTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Effects")
	TObjectPtr<UCurveFloat> DissolveCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Aura|Effects")
	TObjectPtr<UCurveFloat> GlowCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aura|Effects")
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	void Dissolve();

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	UFUNCTION()
	void UpdateGlowMaterial(float GlowValue);

private:
	UPROPERTY(EditAnywhere, Category = "Aura|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};