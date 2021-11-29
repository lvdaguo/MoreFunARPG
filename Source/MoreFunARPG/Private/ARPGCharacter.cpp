#include "ARPGCharacter.h"

AARPGCharacter::AARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetupDataFromDataTable();
}

void AARPGCharacter::ReceiveDamage(const int32 Damage)
{
	if (Damage > 0)
	{
		ChangeHealth(-1 * Damage);
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
