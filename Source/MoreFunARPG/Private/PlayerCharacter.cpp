#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Chaos/KinematicTargets.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Self Define Macro
#define CHECK_DEAD() \
{ \
    if (bIsDead) \
	{ \
		return; \
	} \
}

#define CHECK_DEAD_RETURN_BOOL() \
{ \
	if (bIsDead) \
	{ \
		return false; \
	} \
}

#define CHECK_DEAD_RETURN_INT() \
{ \
	if (bIsDead) \
	{ \
		return -1; \
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

// Life Cycle
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurEnergy = MaxEnergy;

	CurCombo = 0;
	MaxCombo = ComboDamageList.Num();

	TargetMovingSpeed = WalkSpeed;

	bIsAttacking = false;
	bIsHealing = false;
	bIsRolling = false;
	bIsDead = false;
	bIsRunning = false;
	bIsOnHit = false;

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

void APlayerCharacter::LerpSpeed(const float DeltaTime)
{
	if (FMath::IsNearlyEqual(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed))
	{
		return;
	}
	const float Alpha = LerpTime / SwitchRunningTime;
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed, Alpha);
	LerpTime += DeltaTime;
}

void APlayerCharacter::UpdateEnergy(const float DeltaTime)
{
	if (bIsRunning && bIsAttacking == false && IsGettingMovementInput())
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

// Action
void APlayerCharacter::BeginRunning()
{
	CHECK_DEAD()
	// if (CanMove() == false)
	// {
	// 	return;
	// }

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginRunning"))
	bIsRunning = true;
	TargetMovingSpeed = RunningSpeed;
	LerpTime = 0.0f;
}

void APlayerCharacter::EndRunning()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndRunning"))
	bIsRunning = false;
	TargetMovingSpeed = WalkSpeed;
	LerpTime = 0.0f;
}

int32 APlayerCharacter::BeginPrimaryAttack()
{
	CHECK_DEAD_RETURN_INT()

	if (CanMove() == false)
	{
		// ignore new input if player is already doing action
		return -1;
	}

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginPrimaryAttack"))

	bIsAttacking = true;

	// cancel combo reset
	GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);

	const int32 PlayingComboIndex = CurCombo;
	NextCombo();
	return PlayingComboIndex;
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
	CHECK_DEAD_RETURN_BOOL()
	if (CanMove() == false)
	{
		return false;
	}

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginHealing"))
	bIsHealing = true;

	return true;
}

void APlayerCharacter::EndHealing()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndHealing"))

	bIsHealing = false;
}

bool APlayerCharacter::BeginRolling()
{
	CHECK_DEAD_RETURN_BOOL()

	if (bIsRolling)
	{
		return false;
	}

	// interrupt from other states
	bIsHealing = false;
	bIsAttacking = false;
	ResetCombo();

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginRoll"))
	bIsRolling = true;

	return true;
}

void APlayerCharacter::EndRolling()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndRoll"))

	bIsRolling = false;
}

bool APlayerCharacter::BeginOnHit()
{
	// GetMesh()->SetVectorParameterValueOnMaterials()
	CHECK_DEAD_RETURN_BOOL()
	if (bIsOnHit)
	{
		return false;
	}

	if (bIsRunning)
	{
		EndRunning();
	}
	else if (bIsHealing)
	{
		EndHealing();
	}
	else if (bIsAttacking)
	{
		EndPrimaryAttack();
		GetWorldTimerManager().ClearTimer(ComboResetTimerHandle);
		ResetCombo();
	}
	else if (bIsRolling)
	{
		EndRolling();
	}
		

	bIsOnHit = true;
	return true;
}

void APlayerCharacter::EndOnHit()
{
	bIsOnHit = false;
}

// Axis
void APlayerCharacter::MoveForward(const float Value)
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
}

void APlayerCharacter::MoveRight(const float Value)
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
}

void APlayerCharacter::Turn(const float Value)
{
	CHECK_DEAD()

	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(const float Value)
{
	CHECK_DEAD()

	AddControllerPitchInput(Value);
}
