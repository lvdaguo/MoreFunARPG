#include "PlayerCharacterController.h"
#include "PlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCharacterController, Log, All)

APlayerCharacterController::APlayerCharacterController()
{
	Possess(PlayerCharacter);
}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
}

// void APlayerCharacterController::SetupInputComponent()
// {
// 	Super::SetupInputComponent();
//
// 	InputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &APlayerCharacterController::OnPrimaryAttack);
// 	InputComponent->BindAction("Heal", IE_Pressed, this, &APlayerCharacterController::OnHeal);
// 	InputComponent->BindAction("Roll", IE_Pressed, this, &APlayerCharacterController::OnRoll);
// 	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacterController::OnJump);
//
// 	InputComponent->BindAction("Running", IE_Pressed, this, &APlayerCharacterController::OnStartRunning);
// 	InputComponent->BindAction("Running", IE_Released, this, &APlayerCharacterController::OnStopRunning);
//
// 	InputComponent->BindAxis("MoveForward", this, &APlayerCharacterController::OnMoveForward);
// 	InputComponent->BindAxis("MoveRight", this, &APlayerCharacterController::OnMoveRight);
// 	InputComponent->BindAxis("Turn", this, &APlayerCharacterController::OnTurn);
// 	InputComponent->BindAxis("LookUp", this, &APlayerCharacterController::OnLookUp);
// }
//
// void APlayerCharacterController::OnStartRunning()
// {
// 	UE_LOG(LogPlayerCharacterController, Log, TEXT("OnStartRunning"))
// 	PlayerCharacter->OnStartRunning();
// }
//
// void APlayerCharacterController::OnStopRunning()
// {
// 	UE_LOG(LogPlayerCharacterController, Log, TEXT("OnStopRunning"))
// 	PlayerCharacter->OnStopRunning();
// }
//
// void APlayerCharacterController::OnPrimaryAttack()
// {
// 	UE_LOG(LogPlayerCharacterController, Log, TEXT("OnPrimaryAttack"))
// 	PlayerCharacter->OnPrimaryAttack();
// }
//
// void APlayerCharacterController::OnHeal()
// {
// 	UE_LOG(LogPlayerCharacterController, Log, TEXT("OnHeal"))
// 	PlayerCharacter->OnHeal();
// }
//
// void APlayerCharacterController::OnRoll()
// {
// 	UE_LOG(LogPlayerCharacterController, Log, TEXT("OnRoll"))
// 	PlayerCharacter->OnRoll();
// }
//
// void APlayerCharacterController::OnJump()
// {
// 	UE_LOG(LogPlayerCharacterController, Log, TEXT("OnJump"))
// 	PlayerCharacter->OnJump();
// }
//
// void APlayerCharacterController::OnMoveForward(const float Value)
// {
// 	PlayerCharacter->OnMoveRight(Value);
// }
//
// void APlayerCharacterController::OnMoveRight(const float Value)
// {
// 	PlayerCharacter->OnMoveRight(Value);
// }
//
// void APlayerCharacterController::OnTurn(const float Value)
// {
// 	PlayerCharacter->OnTurn(Value);
// }
//
// void APlayerCharacterController::OnLookUp(const float Value)
// {
// 	PlayerCharacter->OnLookUp(Value);
// }
