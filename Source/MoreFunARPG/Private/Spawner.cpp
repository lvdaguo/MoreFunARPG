#include "Spawner.h"
#include "ARPGCharacter/MonsterCharacter.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Constructor
ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->SetupAttachment(RootComponent);
}

// Setup Default
void ASpawner::StartMonsterSpawnRoutine()
{
	constexpr bool Loop = true;
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawner::SpawnMonster, SpawnInterval, Loop);
}

void ASpawner::SetupDelegate()
{
	PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetPawn<APlayerCharacter>();
	PlayerCharacter->PlayerDieEvent().AddUObject(this, &ASpawner::OnPlayerDie);
}

// Life Cycle
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnBox = SpawnVolume->Bounds.GetBox();
	StartMonsterSpawnRoutine();
	SetupDelegate();
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Spawn Operation
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

void ASpawner::SpawnMonster()
{
	AMonsterCharacter* MonsterCharacter = RandomSpawn<AMonsterCharacter>(MonsterClass);
	// may fail because of collision
	if (MonsterCharacter == nullptr)
	{
		return;
	}
	MonsterCharacter->MonsterDieEvent().AddUObject(this, &ASpawner::OnMonsterDie);
}

void ASpawner::SpawnHealPotion(const FVector& Position) const
{
	if (FMath::FRand() <= HealPotionDropRate)
	{
		GetWorld()->SpawnActor<AHealPotion>(HealthPotionClass, Position, FRotator::ZeroRotator);
	}
}

void ASpawner::SpawnBoss()
{
}

// Listener
void ASpawner::OnMonsterDie(const AMonsterCharacter* MonsterCharacter)
{
	SpawnHealPotion(MonsterCharacter->GetActorLocation());
	PlayerExpUpdate.Broadcast(MonsterCharacter->GetExpWorth());
	PlayerScoreUpdate.Broadcast(MonsterCharacter->GetScore());
}

void ASpawner::InvokePlayerRespawn() const
{
	PlayerRespawn.Broadcast();
}

void ASpawner::DelayedPlayerRespawn()
{
	FTimerHandle Handle;
	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ASpawner::InvokePlayerRespawn);
	GetWorldTimerManager().SetTimer(Handle, Delegate, PlayerRespawnDelay, false);
}

void ASpawner::OnPlayerDie()
{
	DelayedPlayerRespawn();
}
