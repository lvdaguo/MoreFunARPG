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

FVector ASpawner::GetRandomPointInBox() const
{
	const float X = FMath::RandRange(SpawnBox.Min.X, SpawnBox.Max.X);
	const float Y = FMath::RandRange(SpawnBox.Min.Y, SpawnBox.Max.Y);
	return FVector(X, Y, SpawnHeight);
}

template<class T>
T* ASpawner::RandomSpawn(UClass * ActorClass)
{
	const FVector Position = GetRandomPointInBox();
	return GetWorld()->SpawnActor<T>(ActorClass, Position, FRotator::ZeroRotator);
}

void ASpawner::RandomSpawnMonster()
{
	AMonsterCharacter* MonsterCharacter = RandomSpawn<AMonsterCharacter>(MonsterClass);
	if (MonsterCharacter == nullptr)
	{
		return;
	}
	MonsterCharacter->MonsterDieEvent().AddUObject(this, &ASpawner::OnMonsterDie);
}

void ASpawner::SpawnHealPotion(const FVector& Position)
{
	GetWorld()->SpawnActor<AHealPotion>(HealthPotionClass, Position, FRotator::ZeroRotator);

}

void ASpawner::OnMonsterDie(const AMonsterCharacter* MonsterCharacter)
{
	SpawnHealPotion(MonsterCharacter->GetActorLocation());
	PlayerExpUpdate.ExecuteIfBound(MonsterCharacter->GetExpWorth());
	PlayerScoreUpdate.ExecuteIfBound(MonsterCharacter->GetScore());
}

void ASpawner::RandomSpawnBoss()
{
}