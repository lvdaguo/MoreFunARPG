#include "ARPGCharacter/MonsterCharacter.h"

#include "Spawner.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Constructor
AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Setup Runtime
void AMonsterCharacter::SetupDataFromDataTable()
{
	check(LevelDataTable != nullptr)

	LevelDataTable->GetAllRows(nullptr, AllLevelData);
	MaxLevel = AllLevelData.Num();
	CurLevelData = nullptr;
}

void AMonsterCharacter::SetupState()
{
	Super::SetupState();
	bIsInvincible = false;
	bIsOnHit = false;
	bIsHealing = false;
	bIsAttacking = false;
	bIsRunning = false;
}

void AMonsterCharacter::SetupRandomMesh()
{
	const int32 Index = FMath::RandHelper(RandomMeshPool.Num());
	GetMesh()->SetSkeletalMesh(RandomMeshPool[Index]);
}

// Level Setup
void AMonsterCharacter::SetupLevelByTime()
{
	int32 Level = 1;
	const float Cur = GetWorld()->TimeSeconds;
	for (int32 i = 0; i < LevelUnlockTimeList.Num(); ++i)
	{
		if (Cur < LevelUnlockTimeList[i])
		{
			break;
		}
		++Level;
	}
	SetupByLevel(Level);
}

void AMonsterCharacter::SetupByLevel(const int32 Level)
{
	CurLevel = Level;
	const int32 LevelIndex = Level - 1;
	CurLevelData = AllLevelData[LevelIndex];
	MaxHealth = CurLevelData->MaxHealth;
	CurHealth = MaxHealth;
}

// Life Cycle
void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupDataFromDataTable();
	
	SetupLevelByTime();
	
	SetupRandomMesh();
	
	TargetMovingSpeed = PatrolSpeed;
	GetCharacterMovement()->MaxWalkSpeed = TargetMovingSpeed;
}

void AMonsterCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	LerpSpeed(DeltaTime);
}

// Running
void AMonsterCharacter::LerpSpeed(const float DeltaTime)
{
	if (FMath::IsNearlyEqual(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed))
	{
		return;
	}
	const float Alpha = LerpTime / SwitchRunningTime;

	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed, Alpha);
	LerpTime += DeltaTime;
}

// States
void AMonsterCharacter::InterruptExistingStates()
{
	EndRunning();
	EndHealing();
	EndAttack();
}

// Behavior Tree Task Helper
bool AMonsterCharacter::BeginAttack()
{
	const bool FailCondition = CanAct() == false;
	return BeginActionBase(bIsAttacking, FailCondition);
}

void AMonsterCharacter::EndAttack()
{
	EndActionBase(bIsAttacking);
}

bool AMonsterCharacter::BeginHealing()
{
	const bool FailCondition = CanAct() == false || HealPotion <= 0;
	return BeginActionBase(bIsHealing, FailCondition);
}

void AMonsterCharacter::EndHealing()
{
	EndActionBase(bIsHealing);
}

bool AMonsterCharacter::BeginRunning()
{
	bIsRunning = true;
	TargetMovingSpeed = ChaseSpeed;
	LerpTime = 0.0f;
	return SUCCESS;
}

void AMonsterCharacter::EndRunning()
{
	bIsRunning = false;
	TargetMovingSpeed = PatrolSpeed;
	LerpTime = 0.0f;
}

bool AMonsterCharacter::BeginOnHit()
{
	InterruptExistingStates();
	bIsOnHit = true;
	
	return SUCCESS;
}

void AMonsterCharacter::EndOnHit()
{
	EndActionBase(bIsOnHit);
}

bool AMonsterCharacter::BeginInvincible()
{
	return BeginActionBase(bIsInvincible, bIsInvincible);
}

void AMonsterCharacter::EndInvincible()
{
	EndActionBase(bIsInvincible);
}

// Weapon
void AMonsterCharacter::EnableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	Super::EnableWeapon(WeaponHitBox);
}

void AMonsterCharacter::DisableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	Super::DisableWeapon(WeaponHitBox);
}

// Override
void AMonsterCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	DealDamageBase(Cast<APlayerCharacter>(OtherActor));
}

void AMonsterCharacter::Die()
{
	Super::Die();
	MonsterDie.Broadcast(this);
	MonsterDie.Clear();
	SetLifeSpan(DeadBodyExistTime);
}

void AMonsterCharacter::ReceiveHeal()
{
	HealPotion--;
	ChangeHealthBase(HealAmount);
}

void AMonsterCharacter::ReceiveDamage(const int32 Damage)
{
	if (bIsInvincible)
	{
		return;
	}
	if (Damage > 0)
	{
		DamageReceived.Broadcast(Damage);
		
		ChangeHealthBase(-1 * Damage);
		if (bIsDead == false)
		{
			OnHit();
		}
	}
}
