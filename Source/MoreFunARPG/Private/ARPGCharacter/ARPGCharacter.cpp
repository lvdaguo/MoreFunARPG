#include "ARPGCharacter/ARPGCharacter.h"

#include "Blueprint/UserWidget.h"

// Constructor
AARPGCharacter::AARPGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AARPGCharacter::SetupDelegate()
{
	HealthChange.AddUObject(this, &AARPGCharacter::OnHealthChange);
}

// Life Cycle
void AARPGCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupState();
	SetupDelegate();
}

// Virtual Base
void AARPGCharacter::ReceiveDamage(const int32 Damage)
{
	if (Damage > 0)
	{
		ChangeHealthBase(-1 * Damage);
		DamageReceived.Broadcast(Damage);
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

// Safe Base
void AARPGCharacter::ChangeHealthBase(const int32 Diff)
{
	const int32 OriginHealth = CurHealth;
	CurHealth = FMath::Clamp(CurHealth + Diff, 0, MaxHealth);
	HealthChange.Broadcast(OriginHealth, CurHealth);
}

void AARPGCharacter::DealDamageBase(AARPGCharacter* Receiver) const
{
	// cast fail or hit self
	if (Receiver == nullptr || Receiver == this)
	{
		return;
	}
	const int32 CalculatedDamage = GetCalculatedDamage();
	Receiver->ReceiveDamage(CalculatedDamage);
}

bool AARPGCharacter::BeginActionBase(bool& OutState, const bool FailCondition)
{
	if (FailCondition)
	{
		return FAIL;
	}
	OutState = true;
	return SUCCESS;
}

void AARPGCharacter::EndActionBase(bool& OutState)
{
	if (OutState == false)
	{
		return;
	}
	OutState = false;
}

// Weapon
void AARPGCharacter::EnableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AARPGCharacter::DisableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Listener
void AARPGCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	DealDamageBase(Cast<AARPGCharacter>(OtherActor));
}

void AARPGCharacter::OnHealthChange(const int32 Before, const int32 After)
{
	if (Before != 0 && After == 0)
	{
		Die();
	}
}
