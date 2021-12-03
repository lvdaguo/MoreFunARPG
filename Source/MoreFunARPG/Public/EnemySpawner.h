#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEnemyDie, int32, ExpWorth, int32, Score);

UCLASS()
class MOREFUNARPG_API AEnemySpawner final : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(const float DeltaTime) override;
	
	template <class T>
	T* RandomSpawnEnemy(UClass* EnemyClass) const;

	// Setting	UPROPERTY(EditDefaultsOnly)
	float SpawnInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 BatchEnemyCount = 1;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AARPGCharacter> MonsterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AARPGCharacter> BossClass;

	UPROPERTY(EditDefaultsOnly)
	int32 BossTriggerCount = 10;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* SpawnVolume;

	UPROPERTY(EditDefaultsOnly)
	float SpawnHeight;

	struct FBox SpawnBox;

	struct FTimerHandle SpawnTimerHandle;

	int32 MonsterCount;

	bool bIsBossSpawned;

	void RandomSpawnMonster();
	void OnMonsterDie(const int32 ExpWorth, const int32 Score);
	void InvokeEnemyDie(int32 ExpWorth, int32 Score);
	void RandomSpawnBoss();

	FEnemyDie EnemyDie;

public:
	FORCEINLINE FEnemyDie& EnemyDieEvent() { return EnemyDie; }
};
