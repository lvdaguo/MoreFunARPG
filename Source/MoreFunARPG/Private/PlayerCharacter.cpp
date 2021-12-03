#include "PlayerCharacter.h"

#include "EnemySpawner.h"
#include "MonsterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Chaos/KinematicTargets.h"
#include "Components/ShapeComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCharacter, Log, All)

// Constructor
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupAttachment();
	SetupComponentDefaultValues();
}

// Setup Default
void APlayerCharacter::SetupComponent()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
}

void APlayerCharacter::SetupAttachment() const
{
	SpringArm->SetupAttachment(GetMesh());
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

void APlayerCharacter::SetupComponentDefaultValues() const
{
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	SpringArm->SetupAttachment(GetMesh());

	SpringArm->bUsePawnControlRotation = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
}

void APlayerCharacter::SetupDataFromDataTable()
{
	check(LevelDataTable != nullptr)

	LevelDataTable->GetAllRows(nullptr, AllLevelData);
	CurLevelData = AllLevelData[0];

	CurHealth = GetMaxHealth();
	CurExpGained = 0;
	MaxLevel = AllLevelData.Num();
	CurLevel = 1;
}

void APlayerCharacter::SetupComboDefaultValues()
{
	CurEnergy = MaxEnergy;
	CurCombo = 0;
	MaxCombo = ComboDamageList.Num();
}

void APlayerCharacter::SetupStateDefaultValues()
{
	bIsAttacking = false;
	bIsComboActive = false;
	bIsHealing = false;
	bIsRolling = false;
	bIsRunning = false;
	bIsOnHit = false;
	bIsMaxRunningSpeed = false;
	bIsInvincible = false;
}

void APlayerCharacter::SetupDelegate()
{
	AActor* Temp = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemySpawner::StaticClass());
	AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(Temp);
	EnemySpawner->EnemyDieEvent().AddDynamic(this, &APlayerCharacter::OnEnemyDie);	
}

void APlayerCharacter::OnEnemyDie(const int32 ExpWorth, const int32 /*Score*/)
{
	ReceiveExp(ExpWorth);
}

// Life Cycle
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupComboDefaultValues();

	TargetMovingSpeed = WalkSpeed;
	VerticalInput = 0.0f;

	SetupDelegate();
}

void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	ExamineRunning();
	LerpSpeed(DeltaTime);
	UpdateEnergy(DeltaTime);
}

// Level
int32 APlayerCharacter::GetAccumulatedExp() const
{
	int32 Result = 0;
	for (int32 i = 0; i < CurLevel - 1; ++i)
	{
		Result += AllLevelData[i]->ExpNeededToNextLevel;
	}
	Result += CurExpGained;
	return Result;
}

void APlayerCharacter::LevelUp()
{
	if (CurLevel == MaxLevel)
	{
		return;
	}
	const int32 CurLevelIndex = CurLevel - 1;
	CurLevelData = AllLevelData[CurLevelIndex + 1];
	CurHealth = GetMaxHealth();
	CurLevel++;
}

void APlayerCharacter::ReceiveExp(const int32 Exp)
{
	check(Exp >= 0)

	CurExpGained += Exp;
	while (CurExpGained >= GetExpNeededToNextLevel())
	{
		if (CurLevel == MaxLevel)
		{
			CurExpGained = GetExpNeededToNextLevel();
			break;
		}
		CurExpGained -= GetExpNeededToNextLevel();
		LevelUp();
	}
}

// Running 
void APlayerCharacter::ExamineRunning()
{
	if (bIsRunning && IsFreeOfAction() && IsMovingForward() == false)
	{
		EndRunning();
	}
}

void APlayerCharacter::LerpSpeed(const float DeltaTime)
{
	if (bIsRunning && CanAct() == false)
	{
		return;
	}
	bIsMaxRunningSpeed = false;
	if (FMath::IsNearlyEqual(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed))
	{
		if (TargetMovingSpeed == RunningSpeed)
		{
			bIsMaxRunningSpeed = true;
		}
		return;
	}
	const float Alpha = LerpTime / SwitchRunningTime;
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed, Alpha);
	LerpTime += DeltaTime;
}

void APlayerCharacter::UpdateEnergy(const float DeltaTime)
{
	if (bIsRunning && IsMovingForward() && IsFreeOfAction())
	{
		// attack can be interrupted now but is still playing the end of the attack animation
		if (bIsComboActive && IsPlayingNetworkedRootMotionMontage())
		{
			StopAnimMontage();
		}
		CurEnergy -= RunningEnergyCost * DeltaTime;
		if (CurEnergy <= 0.0f)
		{
			EndRunning();
		}
	}
	else if (bIsRolling == false)
	{
		if (CurEnergy >= MaxEnergy)
		{
			return;
		}
		CurEnergy += RunningEnergyRefuel * DeltaTime;
	}
}

void APlayerCharacter::Die()
{
	Super::Die();
	PlayerDie.Broadcast(GetAccumulatedExp());
}

// Damage Interface
int32 APlayerCharacter::GetCalculatedDamage() const
{
	return GetNormalDamage();
}

void APlayerCharacter::ReceiveDamage(const int32 Damage)
{
	if (Damage <= 0 || bIsInvincible)
	{
		return;
	}

	const int32 FinalDamage = Damage - GetArmor();
	if (FinalDamage > 0)
	{
		ChangeHealthSafe(-1 * FinalDamage);
		if (bIsDead == false)
		{
			OnHit();
		}
	}
}

void APlayerCharacter::ReceiveHeal()
{
	HealPotion--;
	ChangeHealthSafe(HealAmount);
}

// Action Blueprint Implementation Helper
bool APlayerCharacter::BeginRunning()
{
	if (IsGettingForwardInput() == false)
	{
		return FAIL;
	}
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginRunning"))

	bIsRunning = true;
	TargetMovingSpeed = RunningSpeed;
	LerpTime = 0.0f;

	return SUCCESS;
}

void APlayerCharacter::EndRunning()
{
	if (bIsRunning == false)
	{
		return;
	}
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndRunning"))
	bIsRunning = false;
	TargetMovingSpeed = WalkSpeed;
	LerpTime = 0.0f;
}

bool APlayerCharacter::BeginPrimaryAttack(int32& OutComboAnimIndex)
{
	if (CanAct() == false)
	{
		// ignore new input if player is already doing action
		return FAIL;
	}

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginPrimaryAttack"))

	bIsAttacking = true;

	// cancel combo reset
	GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);

	OutComboAnimIndex = CurCombo;
	GoToNextCombo();

	return SUCCESS;
}

void APlayerCharacter::EndPrimaryAttack()
{
	if (bIsAttacking == false)
	{
		return;
	}
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndPrimaryAttack"))

	bIsAttacking = false;
	bIsComboActive = true;

	// reset combo after some time
	constexpr bool bLoop = false;
	GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this,
	                                &APlayerCharacter::ResetCombo, ComboResetDelayTime, bLoop);
}

bool APlayerCharacter::BeginHealing()
{
	if (CanAct() == false || HealPotion <= 0)
	{
		return FAIL;
	}
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginHealing"))
	bIsHealing = true;

	return SUCCESS;
}

void APlayerCharacter::EndHealing()
{
	if (bIsHealing == false)
	{
		return;
	}
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndHealing"))

	bIsHealing = false;
}

bool APlayerCharacter::BeginRolling()
{
	if (bIsRolling || bIsOnHit || CurEnergy < RollEnergyCost)
	{
		return FAIL;
	}

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginRoll"))

	// interrupt from other states
	EndHealing();
	EndPrimaryAttack();
	GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
	ResetCombo();

	CurEnergy -= RollEnergyCost;
	bIsRolling = true;

	return SUCCESS;
}

void APlayerCharacter::EndRolling()
{
	if (bIsRolling == false)
	{
		return;
	}
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndRoll"))

	bIsRolling = false;
}

bool APlayerCharacter::BeginOnHit()
{
	if (bIsOnHit || bIsAttacking)
	{
		// if doing attacking action
		// the damage still exists but the on hit wont be played
		// means that attacking action wont be interrupted
		return FAIL;
	}
	EndHealing();
	
	bIsOnHit = true;
	return SUCCESS;
}

void APlayerCharacter::EndOnHit()
{
	if (bIsOnHit == false)
	{
		return;
	}
	bIsOnHit = false;
}

bool APlayerCharacter::BeginInvincible()
{
	if (bIsInvincible)
	{
		return FAIL;
	}
	bIsInvincible = true;
	OnInvincibleBegin();

	return SUCCESS;
}

void APlayerCharacter::EndInvincible()
{
	if (bIsInvincible == false)
	{
		return;
	}
	bIsInvincible = false;
	OnInvincibleEnd();
}

// Axis
void APlayerCharacter::MoveForward(const float Value)
{
	VerticalInput = Value;
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(const float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::Turn(const float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(const float Value)
{
	AddControllerPitchInput(Value);
}

// Weapon
void APlayerCharacter::EnableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APlayerCharacter::DisableWeapon(UPrimitiveComponent* WeaponHitBox)
{
	WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
