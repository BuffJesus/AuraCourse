// Not Sure Yet


#include "AI/EQS/AuraEQS_FindRangedAttackPosition.h"

#include "EnvironmentQuery/EnvQueryOption.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_PathingGrid.h"

UAuraEQS_FindRangedAttackPosition::UAuraEQS_FindRangedAttackPosition()
{
	// Create query option
	UEnvQueryOption* Option = CreateDefaultSubobject<UEnvQueryOption>(TEXT("PathingGridOption"));
	
	// Create pathing grid generator
	UEnvQueryGenerator_PathingGrid* PathingGrid = CreateDefaultSubobject<UEnvQueryGenerator_PathingGrid>(TEXT("PathingGridGenerator"));
	
	// Configure the generator/set defaults
	PathingGrid->GridSize.DefaultValue = 1000.f;
	PathingGrid->SpaceBetween.DefaultValue = 100.f;
	PathingGrid->GenerateAround = UEnvQueryContext_Querier::StaticClass();
	
	// Assign Generator to option
	Option->Generator = PathingGrid;
	
	// Add option to query
	Options.Add(Option);
}
