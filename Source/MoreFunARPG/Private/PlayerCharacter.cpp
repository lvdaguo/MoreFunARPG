#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Chaos/KinematicTargets.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Self Define Macro
#define SUCCESS true
#define FAIL false
#define CHECK_DEAD() \
{ \
	if (bIsDead) \
	{ \
		return FAIL; \
	} \
}

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCharacter, Log, All)

// Constructor
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupAttachment();
	SetupComponentDefaultValues();
}

// Init
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

	AllLevelData.Empty();
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
	bIsHealing = false;
	bIsRolling = false;
	bIsDead = false;
	bIsRunning = false;
	bIsOnHit = false;
	bIsMaxRunningSpeed = false;
	bIsInvincible = false;
}

// Life Cycle
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupComboDefaultValues();
	SetupStateDefaultValues();

	TargetMovingSpeed = WalkSpeed;
	
	HorizontalInput = 0.0f;
	VerticalInput = 0.0f;
}

void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	// UE_LOG(LogTemp, Log, TEXT("PlayerStatus: %f, %f, running %d %d"),
	// 	GetCharacterMovement()->MaxWalkSpeed, CurEnergy, bIsRunning, MaxEnergy)
	
	LerpSpeed(DeltaTime);
	UpdateEnergy(DeltaTime);
}

int32 APlayerCharacter::GetDamage() const
{
	return 0;
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

void APlayerCharacter::ReceiveDamage(const int32 Damage)
{
	const int32 FinalDamage = Damage - GetArmor();
	if (FinalDamage > 0)
	{
		ChangeHealth(-1 * FinalDamage);
	}
}

void APlayerCharacter::ReceiveExp(const int32 Exp)
{
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

void APlayerCharacter::LerpSpeed(const float DeltaTime)
{
	bIsMaxRunningSpeed = false;
	if (FMath::IsNearlyEqual(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed))
	{
		if (TargetMovingSpeed == RunningSpeed)
		{
			bIsMaxRunningSpeed = true;
			return;
		}
	}
	const float Alpha = LerpTime / SwitchRunningTime;
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed, Alpha);
	LerpTime += DeltaTime;
}

void APlayerCharacter::UpdateEnergy(const float DeltaTime)
{
	if (bIsRunning && bIsAttacking == false)
	{
		// attack can be interrupted now but is still playing the end of the attack animation
		if (bIsRolling == false && bIsHealing == false &&
			IsPlayingNetworkedRootMotionMontage())
		{
			StopAnimMontage();
		}
		CurEnergy -= RunningEnergyCost * DeltaTime;
		if (CurEnergy <= 0.0f)
		{
			EndRunning();
		}
	}
	else
	{
		if (CurEnergy >= MaxEnergy)
		{
			return;
		}
		CurEnergy += RunningEnergyRefuel * DeltaTime;
	}
}

void APlayerCharacter::InterruptExistingStates()
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
		EndPrimaryAttack();
		GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
		ResetCombo();
	}
	if (bIsRolling)
	{
		EndRolling();
	}
}

// Action
bool APlayerCharacter::BeginRunning()
{
	CHECK_DEAD()
	if (IsGettingMovementInput() == false)
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
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndRunning"))
	bIsRunning = false;
	TargetMovingSpeed = WalkSpeed;
	LerpTime = 0.0f;
}

bool APlayerCharacter::BeginPrimaryAttack(int32& OutComboAnimIndex)
{
	CHECK_DEAD()

	if (CanMove() == false)
	{
		// ignore new input if player is already doing action
		return FAIL;
	}

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginPrimaryAttack"))

	bIsAttacking = true;

	// cancel combo reset
	GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);

	OutComboAnimIndex = CurCombo;
	NextCombo();
	
	return SUCCESS;
}

void APlayerCharacter::EndPrimaryAttack()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndPrimaryAttack"))

	bIsAttacking = false;

	// reset combo after some time
	constexpr bool bLoop = false;
	GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this,
	                                &APlayerCharacter::ResetCombo, ComboResetDelayTime, bLoop);
}

bool APlayerCharacter::BeginHealing()
{
	CHECK_DEAD()
	if (CanMove() == false)
	{
		return FAIL;
	}

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginHealing"))
	bIsHealing = true;

	return SUCCESS;
}

void APlayerCharacter::EndHealing()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndHealing"))

	bIsHealing = false;
}

bool APlayerCharacter::BeginRolling()
{
	CHECK_DEAD()

	if (bIsRolling)
	{
		return FAIL;
	}

	// interrupt from other states
	InterruptExistingStates();
	BeginInvincible();

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginRoll"))
	bIsRolling = true;

	return SUCCESS;
}

void APlayerCharacter::EndRolling()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndRoll"))

	bIsRolling = false;
	EndInvincible();
}

bool APlayerCharacter::BeginOnHit()
{
	CHECK_DEAD()
	if (bIsInvincible)
	{
		return FAIL;
	}

	InterruptExistingStates();
	BeginInvincible();
	
	constexpr bool Loop = false;
	GetWorldTimerManager().SetTimer(InvincibleTimerHandle, this,
		&APlayerCharacter::EndInvincible, DefaultInvisibleTime, Loop);
	
	bIsOnHit = true;
	return SUCCESS;
}

void APlayerCharacter::EndOnHit()
{
	bIsOnHit = false;
}

bool APlayerCharacter::BeginInvincible()
{
	bIsInvincible = true;
	OnInvincibleBegin();

	return SUCCESS;
}

void APlayerCharacter::EndInvincible()
{
	bIsInvincible = false;
	OnInvincibleEnd();
}

// Axis
bool APlayerCharacter::MoveForward(const float Value)
{
	CHECK_DEAD()

	VerticalInput = Value;
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

	return SUCCESS;
}

bool APlayerCharacter::MoveRight(const float Value)
{
	CHECK_DEAD()
	
	HorizontalInput = Value;
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}

	return SUCCESS;
}

bool APlayerCharacter::Turn(const float Value)
{
	CHECK_DEAD()

	AddControllerYawInput(Value);
	return SUCCESS;
}

bool APlayerCharacter::LookUp(const float Value)
{
	CHECK_DEAD()

	AddControllerPitchInput(Value);
	return SUCCESS;
}
