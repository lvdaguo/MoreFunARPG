#include "ARPGCharacter.h"

// DEFINE_LOG_CATEGORY_STATIC(LogARPGCharacter, Log, All)

AARPGCharacter::AARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(LevelDataTable != nullptr)

	AllLevelData.Empty();
	LevelDataTable->GetAllRows(nullptr, AllLevelData);
	CurLevelData = AllLevelData[0];

	CurHealth = GetMaxHealth();
	CurExpGained = 0;
	MaxLevel = AllLevelData.Num();
}

void AARPGCharacter::LevelUp()
{
	if (GetCurLevel() == MaxLevel)
	{
		return;
	}
	const int32 CurLevelIndex = GetCurLevel() - 1;
	CurLevelData = AllLevelData[CurLevelIndex + 1];
	CurHealth = GetMaxHealth();
}

void AARPGCharacter::ReceiveExp(const int32 Exp)
{
	if (GetCurLevel() == MaxLevel)
	{
		CurExpGained = FMath::Min(CurExpGained + Exp, GetExpNeededToNextLevel());
	}
	else
	{
		CurExpGained += Exp;
		while (CurExpGained >= GetExpNeededToNextLevel())
		{
			CurExpGained -= GetExpNeededToNextLevel();
			LevelUp();
		}
	}
}

void AARPGCharacter::ReceiveDamage(const int32 Damage)
{
	const int32 FinalDamage = Damage - GetArmor();
	if (FinalDamage > 0)
	{
		ChangeHealth(-1 * FinalDamage);
	}
}

void AARPGCharacter::ReceiveHeal(const int32 Heal)
{
	ChangeHealth(Heal);
}

void AARPGCharacter::ChangeHealth(const int32 Diff)
{
	const int32 OriginHealth = CurHealth;
	CurHealth = FMath::Clamp(CurHealth + Diff, 0, GetMaxHealth());
	HealthChange.Broadcast(OriginHealth, CurHealth);
}
