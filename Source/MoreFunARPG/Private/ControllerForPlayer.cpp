#include "ControllerForPlayer.h"

#include "PlayerCharacter.h"

void AControllerForPlayer::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	check(PlayerCharacter != nullptr)
}

void AControllerForPlayer::SetupInputComponent()
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
		IE_Pressed, this, &AControllerForPlayer::OnPrimaryAttackPressed);
	Input->BindAction(Heal,
		IE_Pressed, this, &AControllerForPlayer::OnHealPressed);
	Input->BindAction(Running,
		IE_Pressed, this, &AControllerForPlayer::OnRunningPressed);
	Input->BindAction(Running,
		IE_Released, this, &AControllerForPlayer::OnRunningReleased);
	Input->BindAction(Roll,
		IE_Pressed, this, &AControllerForPlayer::OnRollPressed);

	Input->BindAxis(MoveForward,
		this, &AControllerForPlayer::MoveForward);
	Input->BindAxis(MoveRight,
		this, &AControllerForPlayer::MoveRight);
	Input->BindAxis(Turn,
		this, &AControllerForPlayer::Turn);
	Input->BindAxis(LookUp,
		this, &AControllerForPlayer::LookUp);

}

void AControllerForPlayer::OnUnPossess()
{
	Super::OnUnPossess();
	InputComponent->ClearActionBindings();
	InputComponent->AxisBindings.Reset();
}

void AControllerForPlayer::OnPrimaryAttackPressed()
{
	PlayerCharacter->OnPrimaryAttackPressed();	
}

void AControllerForPlayer::OnHealPressed()
{
	PlayerCharacter->OnHealPressed();
}

void AControllerForPlayer::OnRunningPressed()
{
	PlayerCharacter->OnRunningPressed();
}

void AControllerForPlayer::OnRunningReleased()
{
	PlayerCharacter->OnRunningReleased();
}

void AControllerForPlayer::OnRollPressed()
{
	PlayerCharacter->OnRollPressed();
}

void AControllerForPlayer::MoveForward(const float Value)
{
	PlayerCharacter->MoveForward(Value);
}

void AControllerForPlayer::MoveRight(const float Value)
{
	PlayerCharacter->MoveRight(Value);
}

void AControllerForPlayer::Turn(const float Value)
{
	PlayerCharacter->Turn(Value);
}

void AControllerForPlayer::LookUp(const float Value)
{
	PlayerCharacter->LookUp(Value);
}

