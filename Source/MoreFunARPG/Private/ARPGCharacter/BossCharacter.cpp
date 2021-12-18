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
	bIsStunning = false;
}

void ABossCharacter::SetupDelegate()
{
	Super::SetupDelegate();
	
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
	MaxHealth = CurLevelData->MaxHealth;
	CurHealth = MaxHealth;
	UE_LOG(LogTemp, Log, TEXT("Boss Lv: %d"), Level);
}

// Life Cycle
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupDataFromDataTable();
	
	SetupLevelByTime();
	
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
	if (bIsStunning)
	{
		return FAIL;
	}
	InterruptExistingStates();
	bIsStunning = true;
	
	return SUCCESS;
}

void ABossCharacter::EndStun()
{
	EndActionBase(bIsStunning);
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
	if (bIsCharging)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
	Super::EnableWeapon(WeaponHitBox);
}

void ABossCharacter::DisableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	if (bIsCharging)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
	Super::DisableWeapon(WeaponHitBox);
}

void ABossCharacter::OnChargeOverlap(AActor* OtherActor)
{
	if (OtherActor == this)
	{
		return;
	}
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter == nullptr)
	{
		return;
	}

	FVector Origin = this->GetActorLocation();
	FVector Target = OtherActor->GetActorLocation();
	Origin.Z = Target.Z = 0.0f;
	const FVector TargetDirection = (Target - Origin).GetSafeNormal();

	FVector FacingDirection = this->GetActorForwardVector().GetSafeNormal();
	FacingDirection.Z = 0.0f;

	const float Angle = FVector::DotProduct(FacingDirection, TargetDirection);
	const FVector Force = FacingDirection * FMath::Cos(Angle) * ChargeForce;

	PlayerCharacter->GetCharacterMovement()->AddImpulse(Force, true);
	DealDamageBase(Cast<APlayerCharacter>(PlayerCharacter));
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
	BossDie.Broadcast();
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
		DamageReceived.Broadcast(Damage);
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
	const float BeforePercent = static_cast<float>(Before) / MaxHealth;
	const float AfterPercent = static_cast<float>(After) / MaxHealth;
	for (const float Trigger : StunTriggerList)
	{
		if (BeforePercent >= Trigger && AfterPercent < Trigger)
		{
			OnStun();
			break;
		}
	}
}
