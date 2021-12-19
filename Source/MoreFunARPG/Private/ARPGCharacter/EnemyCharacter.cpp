#include "ARPGCharacter/EnemyCharacter.h"

#include "ARPGCharacter/MonsterCharacter.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupAttachment();
	SetupComponentDefaultValues();
}

// Setup Default
void AEnemyCharacter::SetupComponent()
{
	HealthBarTransform = CreateDefaultSubobject<USceneComponent>(TEXT("HealthBarTransform"));
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
}

void AEnemyCharacter::SetupAttachment() const
{
	HealthBarTransform->SetupAttachment(RootComponent);
	HealthBar->SetupAttachment(HealthBarTransform);
}

void AEnemyCharacter::SetupComponentDefaultValues() const
{
	const FVector Location(0.0f, 0.0f, 80.0f);
	const FRotator Rotation(0.0f, 0.0f, 0.0f);
	HealthBarTransform->SetWorldLocationAndRotation(Location, Rotation);

	HealthBar->SetWorldScale3D(FVector(1.0f, 0.2f, 0.2f));
	HealthBar->SetWidgetSpace(EWidgetSpace::World);
}

// Setup Runtime
void AEnemyCharacter::SetupDelegate()
{
	Super::SetupDelegate();
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	PlayerCharacter->PlayerCameraLocationUpdateEvent().AddUObject(this, &AEnemyCharacter::OnPlayerCameraLocationUpdated);
}

// UI
void AEnemyCharacter::BarFacingTarget(const FVector TargetLocation) const
{
	const FVector Start = HealthBarTransform->GetComponentLocation();
	const FVector Target = TargetLocation;
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	HealthBarTransform->SetWorldRotation(Rotation);
}

// Listener
void AEnemyCharacter::OnPlayerCameraLocationUpdated(FVector PlayerCamLocation)
{
	BarFacingTarget(PlayerCamLocation);
}

// Override
void AEnemyCharacter::Die()
{
	Super::Die();
	HealthBar->SetVisibility(false);
}
