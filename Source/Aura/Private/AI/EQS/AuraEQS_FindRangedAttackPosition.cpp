// AuraEQS_FindRangedAttackPosition.cpp
#include "AI/EQS/AuraEQS_FindRangedAttackPosition.h"
#include "EnvironmentQuery/EnvQueryOption.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_PathingGrid.h"

UAuraEQS_FindRangedAttackPosition::UAuraEQS_FindRangedAttackPosition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create the option as an instanced subobject
	UEnvQueryOption* Option = CreateDefaultSubobject<UEnvQueryOption>(TEXT("Option0"));
	
	// Create the generator as an instanced subobject of the option
	UEnvQueryGenerator_PathingGrid* PathingGrid = CreateOptionalDefaultSubobject<UEnvQueryGenerator_PathingGrid>(TEXT("PathingGrid"), Option);
	
	if (PathingGrid)
	{
		// Configure generator defaults (following Epic's pattern)
		PathingGrid->GridSize.DefaultValue = 1000.f;
		PathingGrid->SpaceBetween.DefaultValue = 100.f;
		PathingGrid->GenerateAround = UEnvQueryContext_Querier::StaticClass();
		PathingGrid->ProjectionData.SetNavmeshOnly();
		
		Option->Generator = PathingGrid;
	}
	
	Options.Add(Option);
}