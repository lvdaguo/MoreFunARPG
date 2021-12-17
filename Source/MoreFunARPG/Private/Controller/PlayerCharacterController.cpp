#include "Controller/PlayerCharacterController.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "Spawner.h"
#include "Kismet/GameplayStatics.h"
#include "GlobalNameText.h"

// Life Cycle
void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
	SetupDelegate();
}

// Setup Default
void APlayerCharacterController::SetupDelegate()
{
	ASpawner* Spawner = Cast<ASpawner>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));

	Spawner->PlayerRespawnEvent().AddLambda([this]()
	{
		Possess(PlayerCharacter);
		SetupInputComponent();
	});
}

void APlayerCharacterController::SetupInputAction()
{
	using namespace PlayerCharacterController;
	
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
	using namespace PlayerCharacterController;
	
	InputComponent->BindAxis(MoveForward,
							this, &APlayerCharacterController::OnMoveForward);
	InputComponent->BindAxis(MoveRight,
							this, &APlayerCharacterController::OnMoveRight);
	InputComponent->BindAxis(Turn,
							this, &APlayerCharacterController::OnTurn);
	InputComponent->BindAxis(LookUp,
							this, &APlayerCharacterController::OnLookUp);
}

// Input Component
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

// Override
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

// Binding
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

void APlayerCharacterController::OnMoveForward(const float Value)
{
	PlayerCharacter->MoveForward(Value);
}

void APlayerCharacterController::OnMoveRight(const float Value)
{
	PlayerCharacter->MoveRight(Value);
}

void APlayerCharacterController::OnTurn(const float Value)
{
	PlayerCharacter->Turn(Value);
}

void APlayerCharacterController::OnLookUp(const float Value)
{
	PlayerCharacter->LookUp(Value);
}
