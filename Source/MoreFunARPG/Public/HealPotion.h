#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealPotion.generated.h"

UCLASS()
class MOREFUNARPG_API AHealPotion final : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	AHealPotion();

protected:
	// Setup Default
	void SetupComponent();
	void SetupAttachment();

	// Life Cycle
	virtual void BeginPlay() override;
	
	// Delay Pickup
	void ActivateCollisionDelayed();

	// Component
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* CollisionBox;

	// Property
	UPROPERTY(EditDefaultsOnly)
	float CollisionActivationDelay = 0.5f;
};
