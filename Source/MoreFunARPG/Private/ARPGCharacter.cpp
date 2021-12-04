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
	HealthChange.AddUObject(this, &AARPGCharacter::OnHealthChange);
	
}

void AARPGCharacter::ReceiveDamage(const int32 Damage)
{
	if (Damage > 0)
	{
		ChangeHealthSafe(-1 * Damage);
	}
}

void AARPGCharacter::OnHealthChange(const int32 Before, const int32 After)
{
	UE_LOG(LogTemp, Log, TEXT("HP Bef %d Aft %d"), Before, After)
	if (Before != 0 && After == 0)
	{
		Die();
	}
}

void AARPGCharacter::ChangeHealthSafe(const int32 Diff)
{
	const int32 OriginHealth = CurHealth;
	CurHealth = FMath::Clamp(CurHealth + Diff, 0, GetMaxHealth());
	HealthChange.Broadcast(OriginHealth, CurHealth);

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

void AARPGCharacter::DealDamageSafe(AARPGCharacter* Receiver) const
{
	if (Receiver == nullptr || Receiver == this)
	{
		return;
	}
	const int32 CalculatedDamage = GetCalculatedDamage();
	Receiver->ReceiveDamage(CalculatedDamage);
}

void AARPGCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	DealDamageSafe(Cast<AARPGCharacter>(OtherActor));
}
