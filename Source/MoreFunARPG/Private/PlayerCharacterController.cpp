#include "PlayerCharacterController.h"
#include "PlayerCharacter.h"
#include "Spawner.h"
#include "Kismet/GameplayStatics.h"

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = GetPawn<APlayerCharacter>();

	ASpawner* Spawner = Cast<ASpawner>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	Spawner->PlayerRespawnEvent().AddUObject(this, &APlayerCharacterController::OnPlayerRespawned);
}


void APlayerCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const FName PrimaryAttack(TEXT("PrimaryAttack"));
	const FName Heal(TEXT("Heal"));
	const FName Running(TEXT("Running"));
	const FName Roll(TEXT("Roll"));

	const FName MoveForward(TEXT("MoveForward"));
	const FName MoveRight(TEXT("MoveRight"));
	const FName Turn(TEXT("Turn"));
	const FName LookUp(TEXT("LookUp"));

	UInputComponent* const Input = InputComponent;

	Input->BindAction(PrimaryAttack,
	                  IE_Pressed, this, &APlayerCharacterController::OnPrimaryAttackPressed);
	Input->BindAction(Heal,
	                  IE_Pressed, this, &APlayerCharacterController::OnHealPressed);
	Input->BindAction(Running,
	                  IE_Pressed, this, &APlayerCharacterController::OnRunningPressed);
	Input->BindAction(Running,
	                  IE_Released, this, &APlayerCharacterController::OnRunningReleased);
	Input->BindAction(Roll,
	                  IE_Pressed, this, &APlayerCharacterController::OnRollPressed);

	Input->BindAxis(MoveForward,
	                this, &APlayerCharacterController::MoveForward);
	Input->BindAxis(MoveRight,
	                this, &APlayerCharacterController::MoveRight);
	Input->BindAxis(Turn,
	                this, &APlayerCharacterController::Turn);
	Input->BindAxis(LookUp,
	                this, &APlayerCharacterController::LookUp);
}

void APlayerCharacterController::OnUnPossess()
{
	Super::OnUnPossess();
	InputComponent->ClearActionBindings();
	InputComponent->AxisBindings.Reset();
}

void APlayerCharacterController::OnPlayerRespawned(APlayerCharacter* RespawnedPlayer)
{
	PlayerCharacter->Destroy();
	Possess(RespawnedPlayer);
	PlayerCharacter = RespawnedPlayer;
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
