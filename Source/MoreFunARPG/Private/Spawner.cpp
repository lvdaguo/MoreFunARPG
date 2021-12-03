#include "Spawner.h"

#include "MonsterCharacter.h"
#include "Components/BoxComponent.h"

ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->SetupAttachment(RootComponent);
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	MonsterCount = 0;
	bIsBossSpawned = false;
	
	SpawnBox = SpawnVolume->Bounds.GetBox();

	constexpr bool Loop = true;
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawner::RandomSpawnMonster, SpawnInterval, Loop);
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawner::RandomSpawnMonster()
{
	const float X = FMath::RandRange(SpawnBox.Min.X, SpawnBox.Max.X);
	const float Y = FMath::RandRange(SpawnBox.Min.Y, SpawnBox.Max.Y);
	const FVector Position(X, Y, SpawnHeight);
	AMonsterCharacter* MonsterCharacter = GetWorld()->SpawnActor<AMonsterCharacter>(MonsterClass, Position, FRotator::ZeroRotator);
	if (MonsterCharacter == nullptr)
	{
		return;
	}
	MonsterCharacter->MonsterDieEvent().AddUObject(this, &ASpawner::InvokeEnemyDie);
}

void ASpawner::InvokeEnemyDie(int32 ExpWorth, int32 Score)
{
	UE_LOG(LogTemp, Log, TEXT("Invoked Received %d %d"), ExpWorth, Score)
	EnemyDie.Broadcast(ExpWorth, Score);
}

void ASpawner::RandomSpawnBoss()
{
	
}

// template <class T>
// T* ASpawner::RandomSpawnEnemy(UClass* EnemyClass) const
// {
// 	const float X = FMath::RandRange(SpawnBox.Min.X, SpawnBox.Max.X);
// 	const float Y = FMath::RandRange(SpawnBox.Min.Y, SpawnBox.Max.Y);
// 	const FVector Position(X, Y, SpawnHeight);
// 	return GetWorld()->SpawnActor<T>(EnemyClass, Position, FRotator::ZeroRotator);
// }
