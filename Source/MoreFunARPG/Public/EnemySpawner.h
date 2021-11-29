#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class MOREFUNARPG_API AEnemySpawner final : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

	// Setting
	UPROPERTY(EditDefaultsOnly)
	float SpawnInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 BatchEnemyCount = 1;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> EnemyClass;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* SpawnVolume;

	UPROPERTY(EditDefaultsOnly)
	float SpawnHeight;

	FBox SpawnBox;

	UPROPERTY()
	TSet<AActor*> EnemySet;

	FTimerHandle SpawnTimerHandle;

	void SpawnEnemy();
};
