#pragma once

#include "CoreMinimal.h"
#include "ARPGCharacter/ARPGCharacter.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class MOREFUNARPG_API AEnemyCharacter : public AARPGCharacter
{
	GENERATED_BODY()

protected:
	// Constructor
	AEnemyCharacter();

	// Setup Default
	void SetupComponent();
	void SetupAttachment() const;
	void SetupComponentDefaultValues() const;

	// Setup Runtime
	virtual void SetupDelegate() override;

	// Component
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* HealthBarTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UWidgetComponent* HealthBar;

	// UI
	void BarFacingTarget(FVector TargetLocation) const;

	UFUNCTION()
	void OnPlayerCameraLocationUpdated(FVector PlayerCamLocation);

	// Override
	virtual void Die() override;
};
