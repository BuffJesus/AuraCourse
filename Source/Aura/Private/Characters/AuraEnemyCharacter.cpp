// Not Sure Yet

#include "Characters/AuraEnemyCharacter.h"
#include "Aura/Aura.h"
#include "Tags/AuraTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AuraEnemyAttributeSet.h"
#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widgets/AuraUserWidget.h"

AAuraEnemyCharacter::AAuraEnemyCharacter()
{
    CharacterTags.AddTag(Aura::Entities::Enemy);

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Enemy owns its own ASC
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraEnemyAttributeSet>("AttributeSet");
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
}

void AAuraEnemyCharacter::PossessedBy(AController* NewController)
{
        Super::PossessedBy(NewController);
	
	if (!HasAuthority()) { return; }
	
	AuraAIController = Cast<AAuraAIController>(NewController);
	if (!AuraAIController) { UE_LOG(LogTemp, Error, TEXT("AuraAIController not found!")); return; }
	if (!BehaviorTree) { UE_LOG(LogTemp, Error, TEXT("BehaviorTree not found!")); return; }
	if (!BehaviorTree->BlackboardAsset) { UE_LOG(LogTemp, Error, TEXT("BehaviorTree BlackboardAsset not found!")); return; }
	
	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
	
	// Initialize abilities after ASC is ready
	InitializeAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraEnemyCharacter::BeginPlay()
{
        Super::BeginPlay();
        GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	
	if (UAuraUserWidget* AuraUserWidget { Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()) })
	{
		AuraUserWidget->SetWidgetController(this);
	}
	
	if (const UAuraAttributeSet* AuraAttributeSet { GetAuraAttributeSet() })
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data){ OnHealthChanged.Broadcast(Data.NewValue); }
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data){ OnMaxHealthChanged.Broadcast(Data.NewValue); }
		);
		
		AbilitySystemComponent->RegisterGameplayTagEvent(Aura::Effects::HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AAuraEnemyCharacter::HitReactTagChanged
		);
		
		// Only broadcast initial values if they're valid (replicated on clients)
		// On clients, the delegates above will fire when attributes replicate
		if (HasAuthority())
		{
			OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
			OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
		}
	}
}

void AAuraEnemyCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
        bHitReacting = NewCount > 0;
        GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
 
	if (!HasAuthority()) return;
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
}

void AAuraEnemyCharacter::InitializeAbilityActorInfo()
{
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
        AbilitySystemComponent->AbilityActorInfoSet();
	
	// Only initialize attributes on the server
	if (HasAuthority()) { InitializeDefaultAttributes(); }
}

void AAuraEnemyCharacter::HighlightActor()
{
        GetMesh()->SetRenderCustomDepth(true);
        GetMesh()->SetCustomDepthStencilValue(CustomDepthRed);

        if (Weapon)
        {
                Weapon->SetRenderCustomDepth(true);
                Weapon->SetCustomDepthStencilValue(CustomDepthRed);
        }
}

void AAuraEnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	
	if (Weapon) { Weapon->SetRenderCustomDepth(false); }
}

int32 AAuraEnemyCharacter::GetCharacterLevel() const
{
	return Level;
}

void AAuraEnemyCharacter::Die()
{
        Super::Die();
        SetLifeSpan(LifeSpan);
}

UAnimMontage* AAuraEnemyCharacter::GetAttackMontage_Implementation() const
{
        // Prefer the attack montage array if it has entries.
        if (!AttackMontages.IsEmpty())
        {
                const int32 MontageIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
                return AttackMontages[MontageIndex].Montage;
        }

        // Fallback to the single montage property for backwards compatibility.
        return AttackMontage;
}

void AAuraEnemyCharacter::MulticastHandleDeath_Implementation()
{
        Super::MulticastHandleDeath_Implementation();
	
	// Destroy health bar on all clients
	if (HealthBar) { HealthBar->DestroyComponent(); }
}