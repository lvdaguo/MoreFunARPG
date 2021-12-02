#include "ContentGenerator.h"

#include "MonsterCharacter.h"
#include "Components/BoxComponent.h"

AContentGenerator::AContentGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
}

void AContentGenerator::BeginPlay()
{
	Super::BeginPlay();

	SpawnBox = SpawnVolume->Bounds.GetBox();
	
	constexpr bool Loop = true;
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AContentGenerator::SpawnEnemy, SpawnInterval, Loop);	
}

void AContentGenerator::SpawnEnemy()
{
	const float X = FMath::RandRange(SpawnBox.Min.X, SpawnBox.Max.X);
	const float Y = FMath::RandRange(SpawnBox.Min.Y, SpawnBox.Max.Y);
	const FVector Position(X, Y, SpawnHeight);
	AActor* Enemy = GetWorld()->SpawnActor<AMonsterCharacter>(EnemyClass, Position, FRotator::ZeroRotator);
	EnemySet.Add(Enemy);
}
