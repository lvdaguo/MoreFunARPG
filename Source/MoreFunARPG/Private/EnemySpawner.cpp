#include "EnemySpawner.h"

#include "MonsterCharacter.h"
#include "Components/BoxComponent.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnBox = SpawnVolume->Bounds.GetBox();
	
	constexpr bool Loop = true;
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, Loop);	
}

void AEnemySpawner::SpawnEnemy()
{
	const float X = FMath::RandRange(SpawnBox.Min.X, SpawnBox.Max.X);
	const float Y = FMath::RandRange(SpawnBox.Min.Y, SpawnBox.Max.Y);
	const FVector Position(X, Y, SpawnHeight);
	AActor* Enemy = GetWorld()->SpawnActor<AMonsterCharacter>(EnemyClass, Position, FRotator::ZeroRotator);
	EnemySet.Add(Enemy);
}
