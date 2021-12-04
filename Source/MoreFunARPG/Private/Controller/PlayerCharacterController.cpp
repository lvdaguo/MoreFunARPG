#include "Controller/PlayerCharacterController.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "Spawner.h"
#include "Kismet/GameplayStatics.h"

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
	SetupDelegate();
}

void APlayerCharacterController::SetupDelegate()
{
	ASpawner* Spawner = Cast<ASpawner>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	Spawner->PlayerRespawnEvent().AddUObject(this, &APlayerCharacterController::OnPlayerRespawned);
}

void APlayerCharacterController::SetupInputAction()
{
	static const FName PrimaryAttack(TEXT("PrimaryAttack"));
	static const FName Heal(TEXT("Heal"));
	static const FName Running(TEXT("Running"));
	static const FName Roll(TEXT("Roll"));

	InputComponent->BindAction(PrimaryAttack,
	                           IE_Pressed,
	                           this, &APlayerCharacterController::OnPrimaryAttackPressed);
	InputComponent->BindAction(Heal,
	                           IE_Pressed,
	                           this, &APlayerCharacterController::OnHealPressed);
	InputComponent->BindAction(Running,
	                           IE_Pressed,
	                           this, &APlayerCharacterController::OnRunningPressed);
	InputComponent->BindAction(Running,
	                           IE_Released,
	                           this, &APlayerCharacterController::OnRunningReleased);
	InputComponent->BindAction(Roll,
	                           IE_Pressed,
	                           this, &APlayerCharacterController::OnRollPressed);
}

void APlayerCharacterController::SetupInputAxis()
{
	static const FName MoveForward(TEXT("MoveForward"));
	static const FName MoveRight(TEXT("MoveRight"));
	static const FName Turn(TEXT("Turn"));
	static const FName LookUp(TEXT("LookUp"));
	
	InputComponent->BindAxis(MoveForward,
							this, &APlayerCharacterController::MoveForward);
	InputComponent->BindAxis(MoveRight,
							this, &APlayerCharacterController::MoveRight);
	InputComponent->BindAxis(Turn,
							this, &APlayerCharacterController::Turn);
	InputComponent->BindAxis(LookUp,
							this, &APlayerCharacterController::LookUp);
}

void APlayerCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();
	SetupInputAction();
	SetupInputAxis();
}

void APlayerCharacterController::ClearInputComponent() const
{
	InputComponent->ClearActionBindings();
	InputComponent->AxisBindings.Reset();
}

void APlayerCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PlayerCharacter = Cast<APlayerCharacter>(InPawn);
}

void APlayerCharacterController::OnUnPossess()
{
	Super::OnUnPossess();
	ClearInputComponent();
}

void APlayerCharacterController::OnPlayerRespawned()
{
	Possess(PlayerCharacter);
	SetupInputComponent();
}

void APlayerCharacterController::OnPrimaryAttackPressed()
{
	PlayerCharacter->OnPrimaryAttackPressed();
}

void APlayerCharacterController::OnHealPressed()
{
	PlayerCharacter->OnHealPressed();
}

void APlayerCharacterController::OnRunningPressed()
{
	PlayerCharacter->OnRunningPressed();
}

void APlayerCharacterController::OnRunningReleased()
{
	PlayerCharacter->OnRunningReleased();
}

void APlayerCharacterController::OnRollPressed()
{
	PlayerCharacter->OnRollPressed();
}

void APlayerCharacterController::MoveForward(const float Value)
{
	PlayerCharacter->MoveForward(Value);
}

void APlayerCharacterController::MoveRight(const float Value)
{
	PlayerCharacter->MoveRight(Value);
}

void APlayerCharacterController::Turn(const float Value)
{
	PlayerCharacter->Turn(Value);
}

void APlayerCharacterController::LookUp(const float Value)
{
	PlayerCharacter->LookUp(Value);
}
