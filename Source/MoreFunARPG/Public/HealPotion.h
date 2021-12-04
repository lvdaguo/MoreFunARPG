#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealPotion.generated.h"

UCLASS()
class MOREFUNARPG_API AHealPotion final : public AActor
{
	GENERATED_BODY()

	// Constructor
public:
	AHealPotion();

	// Setup Default
protected:
	void SetupComponent();
	void SetupAttachment();

	// Life Cycle
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// Delay Pickup	
	void DelayPickup();
	void ActivateOverlap() const;

	// Component
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* HitBox;

	// Property
	UPROPERTY(EditDefaultsOnly)
	float ActivateDelay = 0.5f;
};
