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
	CurHealth = FMath::Clamp(CurHealth + Diff, 0, GetMaxHealth());
	HealthChange.Broadcast(OriginHealth, CurHealth);
}

void AARPGCharacter::DealDamageBase(AARPGCharacter* Receiver) const
{
	if (Receiver == nullptr || Receiver == this)
	{
		return;
	}
	const int32 CalculatedDamage = GetCalculatedDamage();
	Receiver->ReceiveDamage(CalculatedDamage);
}

bool AARPGCharacter::BeginActionBase(bool& State, bool FailCondition)
{
	if (FailCondition)
	{
		return FAIL;
	}
	State = true;
	return SUCCESS;
}

void AARPGCharacter::EndActionBase(bool& State)
{
	if (State == false)
	{
		return;
	}
	State = false;
}

// Listener
void AARPGCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	DealDamageBase(Cast<AARPGCharacter>(OtherActor));
}

void AARPGCharacter::OnHealthChange(const int32 Before, const int32 After)
{
	UE_LOG(LogTemp, Log, TEXT("HP Bef %d Aft %d"), Before, After)
	if (Before != 0 && After == 0)
	{
		Die();
	}
}
