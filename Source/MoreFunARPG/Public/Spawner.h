#pragma once

#include "CoreMinimal.h"
#include "MonsterCharacter.h"
#include "HealPotion.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMonsterDie, const class AMonsterCharacter*);
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerRespawn, class APlayerCharacter*)

DECLARE_DELEGATE_OneParam(FPlayerScoreUpdate, int32);
DECLARE_DELEGATE_OneParam(FPlayerExpUpdate, int32);
DECLARE_DELEGATE_OneParam(FPlayerCamLocationUpdate, FVector);

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

	// Setting
	UPROPERTY(EditDefaultsOnly)
	float SpawnInterval = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 BatchEnemyCount = 1;

	UPROPERTY(EditDefaultsOnly)
	float EnemyLevelUpInterval = 30.0f;
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 PlayerLife = 3;

	UPROPERTY(EditDefaultsOnly)
	float PlayerRespawnDelay = 5;

	UPROPERTY(EditDefaultsOnly)
	float SpawnHeight;

	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APlayerCharacter> PlayerCharacterClass;

	struct FBox SpawnBox;

	struct FTimerHandle SpawnTimerHandle;

	int32 MonsterCount;

	bool bIsBossSpawned;
	void RandomSpawnMonster();

	void SpawnHealPotion(const FVector& Position) const;
	UFUNCTION()
	void OnMonsterDie(const AMonsterCharacter* MonsterCharacter);
	void OnPlayerDie(int32 ExpAccumulated);

	void SpawnPlayer(int32 ExpAccumulated);
	void RandomSpawnBoss();

	FPlayerExpUpdate PlayerExpUpdate;
	FPlayerScoreUpdate PlayerScoreUpdate;
	FPlayerRespawn PlayerRespawn;
	FPlayerCamLocationUpdate PlayerCamLocationUpdate;

public:
	FORCEINLINE	FPlayerCamLocationUpdate& PlayerCamLocationUpdateEvent() { return PlayerCamLocationUpdate; }
	FORCEINLINE FPlayerRespawn& PlayerRespawnEvent() { return PlayerRespawn; }
	FORCEINLINE FPlayerExpUpdate& PlayerExpUpdateEvent() { return PlayerExpUpdate; }
	FORCEINLINE FPlayerScoreUpdate& PlayerScoreUpdateEvent() { return PlayerScoreUpdate; }
};
