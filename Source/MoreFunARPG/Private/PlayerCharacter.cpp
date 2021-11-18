#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupAttachment();
	SetupComponentDefaultValues();
	SetupAttributeDefaultValues();
}

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

	MaxEnergy = 100;
	RunEnergyCost = 1;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("Spawn Player!"))
	SetupRuntimeValues();
}

void APlayerCharacter::SetupRuntimeValues()
{
	CurHealth = MaxHealth;
	CurEnergy = MaxEnergy;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &APlayerCharacter::OnPrimaryAttack);
	InputComponent->BindAction("Heal", IE_Pressed, this, &APlayerCharacter::OnHeal);
	InputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacter::OnRoll);
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::OnJump);

	InputComponent->BindAction("Running", IE_Pressed, this, &APlayerCharacter::OnStartRunning);
	InputComponent->BindAction("Running", IE_Released, this, &APlayerCharacter::OnStopRunning);

	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::OnMoveRight);
	InputComponent->BindAxis("Turn", this, &APlayerCharacter::OnTurn);
	InputComponent->BindAxis("LookUp", this, &APlayerCharacter::OnLookUp);
}

void APlayerCharacter::ChangeHealth(int Diff)
{
	const int OriginHealth = CurHealth;
	CurHealth += Diff;
	CurHealth = FMath::Clamp(CurHealth, 0, MaxHealth);

	HealthChange.Broadcast(OriginHealth, CurHealth);
}

void APlayerCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::OnLookUp(const float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::OnStartRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void APlayerCharacter::OnStopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::OnPrimaryAttack()
{
}

void APlayerCharacter::OnHeal()
{
}

void APlayerCharacter::OnRoll()
{
}

void APlayerCharacter::OnJump()
{
	Super::Jump();
}

void APlayerCharacter::OnMoveForward(const float Value)
{
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
	AddControllerYawInput(Value);
}
