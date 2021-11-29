#include "MonsterCharacter.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMonsterCharacter::SetupDataFromDataTable()
{
	check(LevelDataTable != nullptr)

	AllLevelData.Empty();
	LevelDataTable->GetAllRows(nullptr, AllLevelData);
	CurLevelData = AllLevelData[0];

	CurHealth = GetMaxHealth();
	MaxLevel = AllLevelData.Num();
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	const int32 Index = FMath::RandHelper(RandomMeshPool.Num());
	GetMesh()->SetSkeletalMesh(RandomMeshPool[Index]);
}

void AMonsterCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMonsterCharacter::BeginAttack()
{
}

void AMonsterCharacter::EndAttack()
{
}

void AMonsterCharacter::BeginHealing()
{
}

void AMonsterCharacter::EndHealing()
{
}

void AMonsterCharacter::BeginRunning()
{
}

void AMonsterCharacter::EndRunning()
{
}
