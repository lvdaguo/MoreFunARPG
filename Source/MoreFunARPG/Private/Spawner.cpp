#include "Spawner.h"

#include "ARPGCharacter/BossCharacter.h"
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

void ASpawner::SpawnBlockEveryTick()
{
	if (SpawnMaxFail <= 0)
	{
		return;
	}

	if (SceneBlockCount > 0)
	{
		AActor* const SceneBlock = SpawnSceneBlockOnce();
		if (SceneBlock != nullptr)
		{
			SceneBlockCount--;
		}
		else
		{
			SpawnMaxFail--;
		}
	}
}

void ASpawner::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	SpawnBlockEveryTick();
}

// Spawn Operation
void ASpawner::StartMonsterSpawnRoutine()
{
	FTimerHandle SpawnTimerHandle;
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawner::SpawnMonster, MonsterSpawnInterval, true);
}

template <class T>
T* ASpawner::RandomSpawnInBox(UClass* ActorClass)
{
	const FVector Position = FMath::RandPointInBox(SpawnBox);
	return GetWorld()->SpawnActor<T>(ActorClass, Position, FRotator::ZeroRotator);
}

AActor* ASpawner::SpawnSceneBlockOnce() const
{
	FVector Position = FMath::RandPointInBox(SpawnBox);
	Position.Z = SceneBlockPositionZ;

	const FRotator Rotation = FRotator(0.0f, FMath::FRand() * 360.0f, 0.0f);

	// check collision with default scale (biggest width and length)
	AActor* const SceneBlock = GetWorld()->SpawnActor<AActor>(SceneBlockClass, Position, Rotation);

	// if collision test passed
	if (SceneBlock != nullptr)
	{
		auto RandomRange = [](const FFloatRange& Range)
		{
			return FMath::FRandRange
			(
				Range.GetLowerBoundValue(),
				Range.GetUpperBoundValue()
			);
		};

		// set scale with random range
		// must be smaller than the biggest
		const float ScaleX = RandomRange(SceneBlockRangeX);
		const float ScaleY = RandomRange(SceneBlockRangeY);
		const FVector RandomScale = FVector(ScaleX, ScaleY, SceneBlockHeight);
		SceneBlock->SetActorScale3D(RandomScale);
	}

	return SceneBlock;
}

void ASpawner::SpawnMonster()
{
	for (int i = 0; i < BatchEnemyCount;)
	{
		if (SpawnMonsterOnce() != nullptr)
		{
			++i;
		}
	}
}

AMonsterCharacter* ASpawner::SpawnMonsterOnce()
{
	AMonsterCharacter* const MonsterCharacter = RandomSpawnInBox<AMonsterCharacter>(MonsterClass);
	// may fail because of collision
	if (MonsterCharacter != nullptr)
	{
		MonsterCharacter->MonsterDieEvent().AddUObject(this, &ASpawner::OnMonsterDie);
	}
	return MonsterCharacter;
}

void ASpawner::SpawnHealPotionOnce(const FVector& Position) const
{
	if (FMath::FRand() <= HealPotionDropRate)
	{
		GetWorld()->SpawnActor<AHealPotion>(HealthPotionClass, Position, FRotator::ZeroRotator);
	}
}

// Listener
void ASpawner::OnMonsterDie(const AMonsterCharacter* MonsterCharacter)
{
	SpawnHealPotionOnce(MonsterCharacter->GetActorLocation());
	PlayerExpUpdate.Broadcast(MonsterCharacter->GetExpWorth());
	PlayerScoreUpdate.Broadcast(MonsterCharacter->GetScore());

	DeadMonsterCount++;
	if (DeadMonsterCount >= BossSpawnTriggerCount)
	{
		DeadMonsterCount = 0;
		if (bHasBossInScene)
		{
			return;
		}
		ABossCharacter* const BossCharacter = RandomSpawnInBox<ABossCharacter>(BossClass);
		if (BossCharacter == nullptr)
		{
			return;
		}
		bHasBossInScene = true;
		BossCharacter->BossDieEvent().AddLambda([BossCharacter, this]()
		{
			PlayerExpUpdate.Broadcast(BossCharacter->GetExpWorth());
			PlayerScoreUpdate.Broadcast(BossCharacter->GetScoreWorth());
			bHasBossInScene = false;
		});
	}
}

void ASpawner::DelayedPlayerRespawn()
{
	FTimerHandle Handle;
	const FTimerDelegate Delegate = FTimerDelegate::CreateLambda([this]()
	{
		PlayerRespawn.Broadcast();
	});
	GetWorldTimerManager().SetTimer(Handle, Delegate, PlayerRespawnDelay, false);
}

void ASpawner::OnPlayerDie()
{
	DelayedPlayerRespawn();
}
