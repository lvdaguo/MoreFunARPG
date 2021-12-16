#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MoreFunARPGGameMode.generated.h"

UCLASS(minimalapi)
class AMoreFunARPGGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMoreFunARPGGameMode();

protected:
	virtual void BeginPlay() override;
};
