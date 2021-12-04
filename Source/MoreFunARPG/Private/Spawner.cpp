#include "Spawner.h"

#include "MonsterCharacter.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

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

	PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetPawn<APlayerCharacter>();
	check(PlayerCharacter != nullptr)
	PlayerCharacter->PlayerDieEvent().AddUObject(this, &ASpawner::OnPlayerDie);
}

void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PlayerCharacter != nullptr)
	{
		PlayerCamLocationUpdate.ExecuteIfBound(PlayerCharacter->GetCameraWorldLocation());
	}
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

void ASpawner::SpawnHealPotion(const FVector& Position) const
{
	GetWorld()->SpawnActor<AHealPotion>(HealthPotionClass, Position, FRotator::ZeroRotator);
}

void ASpawner::OnMonsterDie(const AMonsterCharacter* MonsterCharacter)
{
	SpawnHealPotion(MonsterCharacter->GetActorLocation());
	if (PlayerExpUpdate.IsBound())
	{
		PlayerExpUpdate.Execute(MonsterCharacter->GetExpWorth());
	}
	if (PlayerScoreUpdate.IsBound())
	{
		PlayerScoreUpdate.Execute(MonsterCharacter->GetScore());
	}
}

void ASpawner::OnPlayerDie(const int32 ExpAccumulated)
{
	if (PlayerLife <= 0)
	{
		return;
	}
	PlayerCharacter = nullptr;
	FTimerHandle Handle;
	const FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ASpawner::SpawnPlayer, ExpAccumulated);
	GetWorldTimerManager().SetTimer(Handle, Delegate, PlayerRespawnDelay, false);
}

void ASpawner::SpawnPlayer(const int32 ExpAccumulated)
{
	PlayerLife--;
	const AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	const FVector Position = PlayerStart->GetActorLocation();
	PlayerCharacter = GetWorld()->SpawnActor<APlayerCharacter>(PlayerCharacterClass, Position, FRotator::ZeroRotator);
	PlayerRespawn.Broadcast(PlayerCharacter);
	if (PlayerExpUpdate.IsBound())
	{
		PlayerExpUpdate.Execute(ExpAccumulated);
	}
}

void ASpawner::RandomSpawnBoss()
{
}