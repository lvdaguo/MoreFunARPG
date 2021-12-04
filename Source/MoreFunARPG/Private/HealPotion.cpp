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
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
}

void AHealPotion::SetupAttachment()
{
	RootComponent = HitBox;
	Mesh->SetupAttachment(RootComponent);
}

// Delay Pickup
void AHealPotion::DelayPickup()
{
	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FTimerHandle ActivateHandle;
	GetWorldTimerManager().SetTimer(ActivateHandle, this,
	                                &AHealPotion::ActivateOverlap, ActivateDelay, false);
}

void AHealPotion::ActivateOverlap() const
{
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Life Cycle
void AHealPotion::BeginPlay()
{
	Super::BeginPlay();
	DelayPickup();
}

void AHealPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
