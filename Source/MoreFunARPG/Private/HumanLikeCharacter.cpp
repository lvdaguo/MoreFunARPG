// #include "HumanLikeCharacter.h"
//
// #include "GameFramework/CharacterMovementComponent.h"
//
// AHumanLikeCharacter::AHumanLikeCharacter()
// {
// 	
// }
//
// void AHumanLikeCharacter::Tick(const float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
// }
//
// void AHumanLikeCharacter::BeginPlay()
// {
// 	Super::BeginPlay();
// }
//
// void AHumanLikeCharacter::SetupStateDefaultValues()
// {
// 	Super::SetupStateDefaultValues();
// }
//
// void AHumanLikeCharacter::LerpSpeed(const float DeltaTime)
// {
// 	if (FMath::IsNearlyEqual(GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed))
// 	{
// 		return;
// 	}
// 	const float Alpha = LerpTime / SwitchRunningTime;
// 	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(
// 		GetCharacterMovement()->MaxWalkSpeed, TargetMovingSpeed, Alpha);
// 	LerpTime += DeltaTime;
// }