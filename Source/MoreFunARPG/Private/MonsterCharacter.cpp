#include "MonsterCharacter.h"

#define SUCCESS true
#define FAIL false

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

bool AMonsterCharacter::BeginAttack()
{

	return SUCCESS;
}

void AMonsterCharacter::EndAttack()
{
}

bool AMonsterCharacter::BeginHealing()
{

	return SUCCESS;
}

void AMonsterCharacter::EndHealing()
{

}

bool AMonsterCharacter::BeginRunning()
{

	return SUCCESS;
}

void AMonsterCharacter::EndRunning()
{
}
