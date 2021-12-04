#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealPotion.generated.h"

UCLASS()
class MOREFUNARPG_API AHealPotion final : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealPotion();

protected:
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* HitBox;

	UPROPERTY(EditDefaultsOnly)
	float ActivateDelay = 0.5f;

	virtual void BeginPlay() override;

	struct FTimerHandle ActivateHandle; 
	
	UFUNCTION()
	void ActivateOverlap();

	virtual void Tick(float DeltaTime) override;

};
