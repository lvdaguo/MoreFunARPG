#include "MoreFunARPGGameMode.h"

AMoreFunARPGGameMode::AMoreFunARPGGameMode()
{
	PlayerLife = 3;
	PlayerRespawnDelay = 3.0f;
	PlayerScore = 0;
}

void AMoreFunARPGGameMode::BeginPlay()
{
	Super::BeginPlay();
}
