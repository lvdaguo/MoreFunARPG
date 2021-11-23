#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
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
	SetupAttributeDefaultValues();
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

void APlayerCharacter::SetupAttributeDefaultValues()
{
	MaxHealth = 100;

	RunningSpeed = 1000.0f;
	WalkSpeed = 600.0f;
	SwitchRunningTime = 1.0f;

	ComboResetDelayTime = 0.8f;

	MaxEnergy = 100;
	RunEnergyCost = 1;
}

void APlayerCharacter::SetupRuntimeValues()
{
	CurHealth = MaxHealth;
	CurEnergy = MaxEnergy;

	CurCombo = 0;
	MaxCombo = ComboDamageList.Num();

	Movement = GetCharacterMovement();

	bIsAttacking = false;
	bIsHealing = false;
	bIsRolling = false;
}

// Life Cycle
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetupRuntimeValues();
}

void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	LerpSpeed(DeltaTime);
}

void APlayerCharacter::LerpSpeed(const float DeltaTime)
{
	if (FMath::IsNearlyEqual(Movement->MaxWalkSpeed, TargetMovingSpeed))
	{
		return;
	}
	const float Alpha = LerpTime / SwitchRunningTime;
	Movement->MaxWalkSpeed = FMath::Lerp(Movement->MaxWalkSpeed, TargetMovingSpeed, Alpha);
	LerpTime += DeltaTime;
}

void APlayerCharacter::ChangeHealth(const int Diff)
{
	const int OriginHealth = CurHealth;
	CurHealth += Diff;
	CurHealth = FMath::Clamp(CurHealth, 0, MaxHealth);

	HealthChange.Broadcast(OriginHealth, CurHealth);
}

// Action
void APlayerCharacter::OnBeginRunning()
{
	CHECK_DEAD()
	
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginRunning"))
	TargetMovingSpeed = RunningSpeed;
	LerpTime = 0.0f;
}

void APlayerCharacter::OnEndRunning()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndRunning"))
	TargetMovingSpeed = WalkSpeed;
	LerpTime = 0.0f;
}

int32 APlayerCharacter::OnBeginPrimaryAttack()
{
	CHECK_DEAD_RETURN_INT()
	
	if (bIsHealing || bIsRolling || bIsAttacking)
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

void APlayerCharacter::OnEndPrimaryAttack()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndPrimaryAttack"))

	bIsAttacking = false;

	// reset combo after some time
	constexpr bool bLoop = false;
	GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this,
	                                &APlayerCharacter::ResetCombo, ComboResetDelayTime, bLoop);
}

bool APlayerCharacter::OnBeginHealing()
{
	CHECK_DEAD_RETURN_BOOL()
	if (bIsAttacking || bIsRolling)
	{
		return false;
	}

	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginHealing"))
	bIsHealing = true;

	return true;
}

void APlayerCharacter::OnEndHealing()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndHealing"))

	bIsHealing = false;
}

bool APlayerCharacter::OnBeginRolling()
{
	CHECK_DEAD_RETURN_BOOL()

	if (bIsRolling)
	{
		return false;
	}
	
	
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnBeginRoll"))
	bIsRolling = true;

	return true;
}

void APlayerCharacter::OnEndRolling()
{
	UE_LOG(LogPlayerCharacter, Log, TEXT("OnEndRoll"))

	bIsRolling = false;
}

// Axis
void APlayerCharacter::OnMoveForward(const float Value)
{
	CHECK_DEAD()
	
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::OnMoveRight(const float Value)
{
	CHECK_DEAD()

	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::OnTurn(const float Value)
{
	CHECK_DEAD()
	
	AddControllerYawInput(Value);
}

void APlayerCharacter::OnLookUp(const float Value)
{
	CHECK_DEAD()
	
	AddControllerPitchInput(Value);
}
