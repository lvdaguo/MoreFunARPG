#pragma once

#include "CoreMinimal.h"
#include "MonsterCharacter.h"
#include "HealPotion.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMonsterDie, const class AMonsterCharacter*)

DECLARE_DELEGATE_OneParam(FPlayerScoreUpdate, int32)
DECLARE_DELEGATE_OneParam(FPlayerExpUpdate, int32)

UCLASS()
class MOREFUNARPG_API ASpawner final : public AActor
{
	GENERATED_BODY()

public:
	ASpawner();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	FVector GetRandomPointInBox() const;
	
	template <class T>
	T* RandomSpawn(class UClass*);

	// template <class T>
	// T* RandomSpawnEnemy(UClass* EnemyClass) const;

	// Setting
	UPROPERTY(EditDefaultsOnly)
	float SpawnInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 BatchEnemyCount = 1;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AMonsterCharacter> MonsterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AARPGCharacter> BossClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHealPotion> HealthPotionClass;
	
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

	void SpawnHealPotion(const FVector& Position);
	UFUNCTION()
	void OnMonsterDie(const AMonsterCharacter* MonsterCharacter);
	void RandomSpawnBoss();

	FPlayerExpUpdate PlayerExpUpdate;
	FPlayerScoreUpdate PlayerScoreUpdate;

public:
	FORCEINLINE FPlayerExpUpdate& PlayerExpUpdateEvent() { return PlayerExpUpdate; }
	FORCEINLINE FPlayerScoreUpdate& PlayerScoreUpdateEvent() { return PlayerScoreUpdate; }
};
