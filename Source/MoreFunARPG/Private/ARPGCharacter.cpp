#include "ARPGCharacter.h"

AARPGCharacter::AARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AARPGCharacter::SetupStateDefaultValues()
{
	bIsDead = false;
}

void AARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupStateDefaultValues();
	SetupDataFromDataTable();
}

void AARPGCharacter::ReceiveDamage(const int32 Damage)
{
	if (Damage > 0)
	{
		ChangeHealthSafe(-1 * Damage);
	}
}

void AARPGCharacter::ReceiveHeal(const int32 Heal)
{
	ChangeHealthSafe(Heal);
}

void AARPGCharacter::ChangeHealthSafe(const int32 Diff)
{
	const int32 OriginHealth = CurHealth;
	CurHealth = FMath::Clamp(CurHealth + Diff, 0, GetMaxHealth());
	if (bIsDead == false && CurHealth == 0)
	{
		Die();
	}
	UE_LOG(LogTemp, Log, TEXT("HP Bef %d Aft %d"), OriginHealth, CurHealth)
	HealthChange.Broadcast(OriginHealth, CurHealth);
}

void AARPGCharacter::Die()
{
	SetActorEnableCollision(false);
	bIsDead = true;
	UnPossessed();
}

void AARPGCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	AARPGCharacter* OtherCharacter = Cast<AARPGCharacter>(OtherActor);
	if (OtherCharacter != nullptr && OtherCharacter != this)
	{
		const int32 CalculatedDamage = GetDamage();
		OtherCharacter->ReceiveDamage(CalculatedDamage);
	}
}
