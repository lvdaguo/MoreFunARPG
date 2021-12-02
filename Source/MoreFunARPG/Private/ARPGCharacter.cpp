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

	HealthChangeEvent().AddUObject(this, &AARPGCharacter::OnHealthChange);

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

void AARPGCharacter::ChangeHealthSafe(const int32 Diff)
{
	const int32 OriginHealth = CurHealth;
	CurHealth = FMath::Clamp(CurHealth + Diff, 0, GetMaxHealth());
	HealthChange.Broadcast(OriginHealth, CurHealth);
}

void AARPGCharacter::OnHealthChange(const int32 Before, const int32 After)
{
	UE_LOG(LogTemp, Log, TEXT("HP Bef %d Aft %d"), Before, After)
	if (Before != 0 && After == 0)
	{
		Die();
	}
}

void AARPGCharacter::Die()
{
	if (bIsDead)
	{
		return;
	}
	SetActorEnableCollision(false);
	bIsDead = true;
	StopAnimMontage();
	GetController()->UnPossess();
}

void AARPGCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	AARPGCharacter* OtherCharacter = Cast<AARPGCharacter>(OtherActor);
	if (OtherCharacter != nullptr && OtherCharacter != this)
	{
		const int32 CalculatedDamage = GetCalculatedDamage();
		OtherCharacter->ReceiveDamage(CalculatedDamage);
	}
}
