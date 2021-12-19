#pragma once

#include "CoreMinimal.h"
#include "HealPotion.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMonsterDie, const class AMonsterCharacter*);
DECLARE_MULTICAST_DELEGATE(FPlayerRespawn);

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerScoreUpdate, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerExpUpdate, int32);

UCLASS()
class MOREFUNARPG_API ASpawner final : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	ASpawner();

	// Setup Runtime
	void SetupDelegate();

protected:
	// Life Cycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Attribute
	UPROPERTY(EditDefaultsOnly)
	float MonsterSpawnInterval = 2.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 BatchEnemyCount = 1;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AMonsterCharacter> MonsterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AARPGCharacter> BossClass;

	UPROPERTY(EditDefaultsOnly)
	int32 BossSpawnTriggerCount = 5;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHealPotion> HealthPotionClass;

	UPROPERTY(EditDefaultsOnly)
	float PlayerRespawnDelay = 5;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APlayerCharacter> PlayerCharacterClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AActor> SceneBlockClass;

	UPROPERTY(EditDefaultsOnly)
	struct FFloatRange SceneBlockRangeX = FFloatRange(5.0f, 10.0f);
	
	UPROPERTY(EditDefaultsOnly)
	struct FFloatRange SceneBlockRangeY = FFloatRange(5.0f, 10.0f);

	UPROPERTY(EditDefaultsOnly)
	float SceneBlockPositionZ = 330.0f;
	
	UPROPERTY(EditDefaultsOnly)
	int SceneBlockCount = 10;
	
	UPROPERTY(EditDefaultsOnly)
	float SceneBlockHeight = 4.0f;
	
	UPROPERTY(EditDefaultsOnly)
	float HealPotionDropRate = 0.25;
	
	// Component
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* SpawnVolume;

	// Runtime Member
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	struct FBox SpawnBox;
	struct FTimerHandle SceneBlockSpawnTimerHandle;
	
	int SpawnMaxFail = 100;
	int DeadMonsterCount = 0;
	bool bHasBossInScene = false;

	// Spawn
	void StartMonsterSpawnRoutine();

	template <class T>
	T* RandomSpawnInBox(UClass* ActorClass);

	AActor* SpawnSceneBlockOnce() const;
	void SpawnBlockEveryTick();

	void SpawnMonster();
	AMonsterCharacter* SpawnMonsterOnce();
	void SpawnHealPotionOnce(const FVector& Position) const;
	
	// Listener
	UFUNCTION()
	void OnMonsterDie(const AMonsterCharacter* MonsterCharacter);

	UFUNCTION()
	void OnPlayerDie();

	UFUNCTION()
	void OnGameOver();
	
	// Delegate
	FPlayerExpUpdate PlayerExpUpdate;
	FPlayerScoreUpdate PlayerScoreUpdate;
	FPlayerRespawn PlayerRespawn;

public:
	FORCEINLINE FPlayerRespawn& PlayerRespawnEvent() { return PlayerRespawn; }
	FORCEINLINE FPlayerExpUpdate& PlayerExpUpdateEvent() { return PlayerExpUpdate; }
	FORCEINLINE FPlayerScoreUpdate& PlayerScoreUpdateEvent() { return PlayerScoreUpdate; }
};
