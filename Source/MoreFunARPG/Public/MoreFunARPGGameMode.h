#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MoreFunARPGGameMode.generated.h"

UCLASS(minimalapi)
class AMoreFunARPGGameMode final : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	int32 PlayerLife = 3;

	UPROPERTY(EditDefaultsOnly)
	float PlayerRespawnDelay = 5;

	int32 PlayerScore;

public:
	AMoreFunARPGGameMode();

protected:
	virtual void BeginPlay() override;
};
