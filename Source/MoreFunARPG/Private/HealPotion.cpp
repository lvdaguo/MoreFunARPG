#include "HealPotion.h"
#include "Components/BoxComponent.h"

// Constructor
AHealPotion::AHealPotion()
{
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
	SetupAttachment();
}

// Setup Default
void AHealPotion::SetupComponent()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
}

void AHealPotion::SetupAttachment()
{
	RootComponent = CollisionBox;
	Mesh->SetupAttachment(RootComponent);
}

// Delay Pickup
void AHealPotion::ActivateCollisionDelayed()
{
	// disable collision and enable it after a while
	// to avoid being picked up by player instantly
	
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FTimerHandle ActivateHandle;
	GetWorldTimerManager().SetTimer(ActivateHandle, FTimerDelegate::CreateLambda([this]()
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}), CollisionActivationDelay, false);
}

// Life Cycle
void AHealPotion::BeginPlay()
{
	Super::BeginPlay();
	ActivateCollisionDelayed();
}
