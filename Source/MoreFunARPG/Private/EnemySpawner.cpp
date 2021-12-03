#include "EnemySpawner.h"

#include "MonsterCharacter.h"
#include "Components/BoxComponent.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->SetupAttachment(RootComponent);
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	MonsterCount = 0;
	bIsBossSpawned = false;
	
	SpawnBox = SpawnVolume->Bounds.GetBox();

	constexpr bool Loop = true;
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::RandomSpawnMonster, SpawnInterval, Loop);
}

void AEnemySpawner::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

template<typename T>
T* AEnemySpawner::RandomSpawnEnemy(UClass* EnemyClass) const
{
	const float X = FMath::RandRange(SpawnBox.Min.X, SpawnBox.Max.X);
	const float Y = FMath::RandRange(SpawnBox.Min.Y, SpawnBox.Max.Y);
	const FVector Position(X, Y, SpawnHeight);
	return GetWorld()->SpawnActor<T>(EnemyClass, Position, FRotator::ZeroRotator);
}

void AEnemySpawner::RandomSpawnMonster()
{
	AMonsterCharacter* MonsterCharacter = RandomSpawnEnemy<AMonsterCharacter>(MonsterClass);
	MonsterCharacter->MonsterDieEvent().AddDynamic(this, &AEnemySpawner::InvokeEnemyDie);
	// MonsterCharacter->MonsterDieEvent().AddDynamic(this, &AEnemySpawner::OnMonsterDie);
	if (MonsterCount > BossTriggerCount && bIsBossSpawned == false)
	{
		RandomSpawnBoss();
	}
}

void AEnemySpawner::OnMonsterDie(const int32 /*ExpWorth*/, const int32 /*Score*/)
{
}

void AEnemySpawner::InvokeEnemyDie(const int32 ExpWorth, const int32 Score)
{
	EnemyDie.Broadcast(ExpWorth, Score);
}

void AEnemySpawner::RandomSpawnBoss()
{
	
}
