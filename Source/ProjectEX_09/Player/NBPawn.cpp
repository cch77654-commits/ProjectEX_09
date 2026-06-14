// NBPawn.cpp


#include "NBPawn.h"
#include "ProjectEX_09.h"


void ANBPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = ProjectEX_09FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NBPawn::BeginPlay() %s [%s]"), *ProjectEX_09FunctionLibrary::GetNetModeString(this), *NetRoleString);
	ProjectEX_09FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ANBPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = ProjectEX_09FunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("NBPawn::PossessedBy() %s [%s]"), *ProjectEX_09FunctionLibrary::GetNetModeString(this), *NetRoleString);
	ProjectEX_09FunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
