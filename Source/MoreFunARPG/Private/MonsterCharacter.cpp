#include "MonsterCharacter.h"

#include "PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AMonsterCharacter::AMonsterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthBarTransform = CreateDefaultSubobject<USceneComponent>(TEXT("HealthBarTransform"));
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	
	HealthBarTransform->SetupAttachment(RootComponent);
	HealthBar->SetupAttachment(HealthBarTransform);

	const FVector Location(0.0f, 0.0f, 80.0f);
	const FRotator Rotation(0.0f, 0.0f, 0.0f);
	HealthBarTransform->SetWorldLocationAndRotation(Location, Rotation);

	HealthBar->SetWorldScale3D(FVector(1.0f, 0.2f, 0.2f));
	HealthBar->SetWidgetSpace(EWidgetSpace::World);
}

void AMonsterCharacter::SetupDataFromDataTable()
{
	check(LevelDataTable != nullptr)

	LevelDataTable->GetAllRows(nullptr, AllLevelData);
	MaxLevel = AllLevelData.Num();
	SetupByLevel(1);
}

void AMonsterCharacter::SetupByLevel(const int32 Level)
{
	CurLevel = Level;
	const int32 LevelIndex = Level - 1;
	CurLevelData = AllLevelData[LevelIndex];
	CurHealth = GetMaxHealth();
}

void AMonsterCharacter::SetupStateDefaultValues()
{
	bIsInvincible = false;
	bIsOnHit = false;
}

void AMonsterCharacter::BeginPlay()
{
	Super::BeginPlay();
	const int32 Index = FMath::RandHelper(RandomMeshPool.Num());
	GetMesh()->SetSkeletalMesh(RandomMeshPool[Index]);

	PlayerCharacter = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	TargetMovingSpeed = PatrolSpeed;
	GetCharacterMovement()->MaxWalkSpeed = TargetMovingSpeed;
}

void AMonsterCharacter::BarFacingPlayer() const
{
	const FVector Start = HealthBarTransform->GetComponentLocation();
	const FVector Target = PlayerCharacter->GetCameraWorldLocation();
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	HealthBarTransform->SetWorldRotation(Rotation);
}

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

void AMonsterCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead)
	{
		return;
	}
	
	BarFacingPlayer();
	LerpSpeed(DeltaTime);
}

void AMonsterCharacter::InterruptExistingStates()
{
	if (bIsRunning)
	{
		EndRunning();
	}
	if (bIsHealing)
	{
		EndHealing();
	}
	if (bIsAttacking)
	{
		EndAttack();
	}
}

bool AMonsterCharacter::BeginAttack()
{
	bIsAttacking = true;

	return SUCCESS;
}

void AMonsterCharacter::EndAttack()
{
	bIsAttacking = false;
}

bool AMonsterCharacter::BeginHealing()
{
	bIsHealing = true;
	
	return SUCCESS;
}

void AMonsterCharacter::EndHealing()
{
	bIsHealing = false;
}

bool AMonsterCharacter::BeginRunning()
{
	CHECK_DEAD()

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
	CHECK_DEAD()
	
	InterruptExistingStates();
	bIsOnHit = true;
	BeginInvincible();

	constexpr bool Loop = false;
	GetWorldTimerManager().SetTimer(InvincibleTimerHandle, this,
		&AMonsterCharacter::EndInvincible, DefaultInvisibleTime, Loop);
	
	return SUCCESS;
}

void AMonsterCharacter::EndOnHit()
{
	bIsOnHit = false;
}

bool AMonsterCharacter::BeginInvincible()
{
	bIsInvincible = true;

	return SUCCESS;
}

void AMonsterCharacter::EndInvincible()
{
	bIsInvincible = false;
}

void AMonsterCharacter::Die()
{
	Super::Die();
	HealthBar->SetVisibility(false);
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
		OnHit();
	}
}
