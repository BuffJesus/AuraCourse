// Not Sure Yet


#include "AbilitySystem/AuraAbilitySystemComponent.h"


// Sets default values for this component's properties
UAuraAbilitySystemComponent::UAuraAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAuraAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAuraAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

