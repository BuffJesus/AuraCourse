// Not Sure Yet

#include "Characters/AuraEnemyCharacter.h"
#include "Aura/Aura.h"
#include "Tags/AuraTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraEnemyAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widgets/AuraUserWidget.h"

AAuraEnemyCharacter::AAuraEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Enemy owns its own ASC
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraEnemyAttributeSet>("AttributeSet");
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
	HealthBar->SetWidgetSpace(EWidgetSpace::Screen);
}

void AAuraEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitializeAbilityActorInfo();
	
	if (HasAuthority()) { AddCharacterAbilities(); }
	
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
	bHasHitReactTag = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHasHitReactTag ? 0.f : BaseWalkSpeed;
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
	
	if (Weapon) { Weapon->SetRenderCustomDepth(true); }
	Weapon->SetCustomDepthStencilValue(CustomDepthRed);
}

void AAuraEnemyCharacter::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	
	if (Weapon) { Weapon->SetRenderCustomDepth(false); }
}

int32 AAuraEnemyCharacter::GetPlayerLevel() const
{
	return Level;
}

void AAuraEnemyCharacter::Die()
{
	Super::Die();
	SetLifeSpan(LifeSpan);
}

void AAuraEnemyCharacter::MulticastHandleDeath_Implementation()
{
	Super::MulticastHandleDeath_Implementation();
	
	// Destroy health bar on all clients
	if (HealthBar) { HealthBar->DestroyComponent(); }
}