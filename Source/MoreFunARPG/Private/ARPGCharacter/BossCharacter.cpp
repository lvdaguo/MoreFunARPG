#include "ARPGCharacter/BossCharacter.h"
#include "Spawner.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Constructor
ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupAttachment();
	SetupComponentDefaultValues();
}

// Setup Default
void ABossCharacter::SetupComponent()
{
	HealthBarTransform = CreateDefaultSubobject<USceneComponent>(TEXT("HealthBarTransform"));
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
}

void ABossCharacter::SetupAttachment() const
{
	HealthBarTransform->SetupAttachment(RootComponent);
	HealthBar->SetupAttachment(HealthBarTransform);
}

void ABossCharacter::SetupComponentDefaultValues() const
{
	const FVector Location(0.0f, 0.0f, 80.0f);
	HealthBarTransform->SetWorldLocationAndRotation(Location, FRotator::ZeroRotator);
	const FVector Scale(1.0f, 0.2f, 0.2f);
	HealthBar->SetWorldScale3D(Scale);
	HealthBar->SetWidgetSpace(EWidgetSpace::World);
}

// Setup Runtime
void ABossCharacter::SetupDataFromDataTable()
{
	LevelDataTable->GetAllRows(nullptr, AllLevelData);
	MaxLevel = AllLevelData.Num();
	CurLevelData = nullptr;
}

void ABossCharacter::SetupState()
{
	Super::SetupState();
	bIsMeleeAttacking = false;
	bIsRangeAttacking = false;
	bIsCharging = false;
	bIsInvincible = false;
	bIsStun = false;
}

void ABossCharacter::SetupDelegate()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	PlayerCharacter->PlayerCameraLocationUpdateEvent().AddUObject(this, &ABossCharacter::OnPlayerCameraLocationUpdated);
}

// Level Setup
void ABossCharacter::SetupLevelByTime()
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

void ABossCharacter::SetupByLevel(const int32 Level)
{
	CurLevel = Level;
	const int32 LevelIndex = Level - 1;
	CurLevelData = AllLevelData[LevelIndex];
	CurHealth = GetMaxHealth();
	UE_LOG(LogTemp, Log, TEXT("Boss Lv: %d"), Level);
}

// Life Cycle
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupDataFromDataTable();
	
	SetupLevelByTime();
	SetupDelegate();
	
	TargetMovingSpeed = Speed;
	GetCharacterMovement()->MaxWalkSpeed = TargetMovingSpeed;
}

void ABossCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// UI
void ABossCharacter::BarFacingTarget(const FVector TargetLocation) const
{
	const FVector Start = HealthBarTransform->GetComponentLocation();
	const FVector Target = TargetLocation;
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	HealthBarTransform->SetWorldRotation(Rotation);
}

// States
void ABossCharacter::InterruptExistingStates()
{
	EndMeleeAttack();
	EndRangeAttack();
	EndCharge();
}

// Behavior Tree Task Helper
bool ABossCharacter::BeginMeleeAttack()
{
	const bool FailCondition = CanAct() == false;
	return BeginActionBase(bIsMeleeAttacking, FailCondition);
}

void ABossCharacter::EndMeleeAttack()
{
	EndActionBase(bIsMeleeAttacking);
}

bool ABossCharacter::BeginRangeAttack()
{
	const bool FailCondition = CanAct() == false;
	return BeginActionBase(bIsRangeAttacking, FailCondition);
}

void ABossCharacter::EndRangeAttack()
{
	EndActionBase(bIsRangeAttacking);
}

bool ABossCharacter::BeginCharge()
{
	const bool FailCondition = CanAct() == false;
	return BeginActionBase(bIsCharging, FailCondition);
}

void ABossCharacter::EndCharge()
{
	EndActionBase(bIsCharging);
}

bool ABossCharacter::BeginStun()
{
	if (bIsStun)
	{
		return FAIL;
	}
	InterruptExistingStates();
	bIsStun = true;
	
	return SUCCESS;
}

void ABossCharacter::EndStun()
{
	EndActionBase(bIsStun);
}

bool ABossCharacter::BeginInvincible()
{
	return BeginActionBase(bIsInvincible, bIsInvincible);
}

void ABossCharacter::EndInvincible()
{
	EndActionBase(bIsInvincible);
}

// Weapon
void ABossCharacter::EnableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABossCharacter::DisableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABossCharacter::OnChargeOverlap(AARPGCharacter* Character)
{
	if (Character == this)
	{
		return;
	}
	const FVector Arrow = Character->GetActorLocation() - this->GetActorLocation();
	const FVector Direction = Arrow.GetSafeNormal();
	const FVector Force = Direction * ChargeForce;
	Character->GetCapsuleComponent()->AddImpulse(Force);
	DealDamageBase(Cast<APlayerCharacter>(Character));
}

// Override
int32 ABossCharacter::GetCalculatedDamage() const
{
	int32 FinalDamage;
	if (bIsMeleeAttacking)
	{
		FinalDamage = GetMeleeDamage();
	}
	else if (bIsRangeAttacking)
	{
		FinalDamage = GetRangeDamage();
	}
	else
	{
		FinalDamage = GetChargeDamage();
	}
	return FinalDamage;
}

void ABossCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	DealDamageBase(Cast<APlayerCharacter>(OtherActor));
}

void ABossCharacter::Die()
{
	Super::Die();
	HealthBar->SetVisibility(false);
	BossDie.Broadcast(this);
	BossDie.Clear();
	SetLifeSpan(DeadBodyExistTime);
}

void ABossCharacter::ReceiveDamage(const int32 Damage)
{
	if (bIsInvincible)
	{
		return;
	}
	if (Damage > 0)
	{
		ChangeHealthBase(-1 * Damage);
	}
}

// Listener
void ABossCharacter::OnPlayerCameraLocationUpdated(FVector PlayerCamLocation)
{
	BarFacingTarget(PlayerCamLocation);
}

void ABossCharacter::OnHealthChange(const int32 Before, const int32 After)
{
	Super::OnHealthChange(Before, After);
	for (const int32 Trigger : StunTriggerList)
	{
		if (Before >= Trigger && After < Trigger)
		{
			OnStun();
			break;
		}
	}
}
