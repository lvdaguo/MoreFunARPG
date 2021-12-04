#include "ARPGCharacter/MonsterCharacter.h"

#include "Spawner.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Constructor
AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupAttachment();
	SetupComponentDefaultValues();
}

// Setup Default
void AMonsterCharacter::SetupComponent()
{
	HealthBarTransform = CreateDefaultSubobject<USceneComponent>(TEXT("HealthBarTransform"));
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
}

void AMonsterCharacter::SetupAttachment() const
{
	HealthBarTransform->SetupAttachment(RootComponent);
	HealthBar->SetupAttachment(HealthBarTransform);
}

void AMonsterCharacter::SetupComponentDefaultValues() const
{
	const FVector Location(0.0f, 0.0f, 80.0f);
	const FRotator Rotation(0.0f, 0.0f, 0.0f);
	HealthBarTransform->SetWorldLocationAndRotation(Location, Rotation);

	HealthBar->SetWorldScale3D(FVector(1.0f, 0.2f, 0.2f));
	HealthBar->SetWidgetSpace(EWidgetSpace::World);
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

void AMonsterCharacter::SetupDelegate()
{
	ASpawner* Spawner = Cast<ASpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	Spawner->PlayerCamLocationUpdateEvent().AddUObject(this, &AMonsterCharacter::OnPlayerCamLocationUpdated);
}

void AMonsterCharacter::RandomSetupMesh()
{
	const int32 Index = FMath::RandHelper(RandomMeshPool.Num());
	GetMesh()->SetSkeletalMesh(RandomMeshPool[Index]);
}

// Level Setup
void AMonsterCharacter::SetupByLevel(const int32 Level)
{
	CurLevel = Level;
	const int32 LevelIndex = Level - 1;
	CurLevelData = AllLevelData[LevelIndex];
	CurHealth = GetMaxHealth();
}

// Life Cycle
void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupDataFromDataTable();

	// To DO: better setup by spawner  
	if (CurLevelData == nullptr)
	{
		SetupByLevel(1);
	}
	
	RandomSetupMesh();
	SetupDelegate();
	
	TargetMovingSpeed = PatrolSpeed;
	GetCharacterMovement()->MaxWalkSpeed = TargetMovingSpeed;
}

void AMonsterCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	LerpSpeed(DeltaTime);
}

// UI
void AMonsterCharacter::BarFacingTarget(const FVector TargetLocation) const
{
	const FVector Start = HealthBarTransform->GetComponentLocation();
	const FVector Target = TargetLocation;
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	HealthBarTransform->SetWorldRotation(Rotation);
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
	if (CanAct() == false)
	{
		return FAIL;
	}
	bIsAttacking = true;

	return SUCCESS;
}

void AMonsterCharacter::EndAttack()
{
	if (bIsAttacking == false)
	{
		return;
	}
	bIsAttacking = false;
}

bool AMonsterCharacter::BeginHealing()
{
	if (CanAct() == false || HealPotion <= 0)
	{
		return FAIL;
	}
	bIsHealing = true;
	return SUCCESS;
}

void AMonsterCharacter::EndHealing()
{
	if (bIsHealing == false)
	{
		return;
	}
	bIsHealing = false;
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
	if (bIsOnHit)
	{
		return FAIL;
	}
	InterruptExistingStates();
	bIsOnHit = true;
	
	return SUCCESS;
}

void AMonsterCharacter::EndOnHit()
{
	if (bIsOnHit == false)
	{
		return;
	}
	bIsOnHit = false;
}

bool AMonsterCharacter::BeginInvincible()
{
	if (bIsInvincible)
	{
		return FAIL;
	}
	
	bIsInvincible = true;

	return SUCCESS;
}

void AMonsterCharacter::EndInvincible()
{
	if (bIsInvincible == false)
	{
		return;
	}
	bIsInvincible = false;
}

// Weapon
void AMonsterCharacter::EnableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMonsterCharacter::DisableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Override
void AMonsterCharacter::OnWeaponOverlap(AActor* OtherActor)
{
	DealDamageSafe(Cast<APlayerCharacter>(OtherActor));
}

void AMonsterCharacter::Die()
{
	Super::Die();
	HealthBar->SetVisibility(false);
	MonsterDie.Broadcast(this);
	MonsterDie.Clear();
}

void AMonsterCharacter::ReceiveHeal()
{
	HealPotion--;
	ChangeHealthSafe(HealAmount);
}

void AMonsterCharacter::ReceiveDamage(const int32 Damage)
{
	if (bIsInvincible)
	{
		return;
	}
	if (Damage > 0)
	{
		ChangeHealthSafe(-1 * Damage);
		if (bIsDead == false)
		{
			OnHit();
		}
	}
}

// Listener
void AMonsterCharacter::OnPlayerCamLocationUpdated(FVector PlayerCamLocation)
{
	BarFacingTarget(PlayerCamLocation);
}
