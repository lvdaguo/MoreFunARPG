#include "MonsterCharacter.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterCharacter::Tick(float DeltaTime)
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
