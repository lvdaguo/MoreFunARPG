#include "HealPotion.h"

#include "Components/BoxComponent.h"

AHealPotion::AHealPotion()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));

	RootComponent = HitBox;
	Mesh->SetupAttachment(RootComponent);

}

void AHealPotion::BeginPlay()
{
	Super::BeginPlay();

	HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWorldTimerManager().SetTimer(ActivateHandle, this,
	                                &AHealPotion::ActivateOverlap, ActivateDelay, false);
}

void AHealPotion::ActivateOverlap()
{
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AHealPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
