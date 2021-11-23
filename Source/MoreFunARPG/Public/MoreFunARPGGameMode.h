#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MoreFunARPGGameMode.generated.h"

UCLASS(minimalapi)
class AMoreFunARPGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	int32 PlayerLife;

	UPROPERTY(EditDefaultsOnly)
	float PlayerRespawnDelay;

	int32 PlayerScore;

	AMoreFunARPGGameMode();

protected:
	virtual void BeginPlay() override;
};
