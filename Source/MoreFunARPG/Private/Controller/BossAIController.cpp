#include "Controller/BossAIController.h"

#include "Controller/MonsterAIController.h"

#include "Spawner.h"
#include "ARPGCharacter/BossCharacter.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

ABossAIController::ABossAIController()
{
}

void ABossAIController::SetupDelegate()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	check(PlayerCharacter != nullptr)
	PlayerCharacter->PlayerDieEvent().AddUObject(this, &ABossAIController::OnPlayerDead);

	ASpawner* Spawner = Cast<ASpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	Spawner->PlayerRespawnEvent().AddUObject(this, &ABossAIController::OnPlayerRespawn);
}

void ABossAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviourTree);

	OnPlayerRespawn();
	SetupDelegate();

	AActor* PlayerCharacter = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerActor"), PlayerCharacter);

	GetBlackboardComponent()->SetValueAsBool(TEXT("IsChargeReady"), true);
}

void ABossAIController::OnPlayerDead()
{
	static const FName IsPlayerDead(TEXT("IsPlayerDead"));
	GetBlackboardComponent()->SetValueAsBool(IsPlayerDead, true);
}

void ABossAIController::OnPlayerRespawn()
{
	static const FName IsPlayerDead(TEXT("IsPlayerDead"));
	GetBlackboardComponent()->SetValueAsBool(IsPlayerDead, false);
}

void ABossAIController::StartChargeCoolDownCountDown()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABossAIController::SetChargeReady, BossCharacter->GetChargeCoolDownTime(), false);
}

void ABossAIController::SetChargeReady()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsChargeReady"), true);
}

void ABossAIController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const AActor* PlayerCharacter = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerActor")));
	const float Distance = (BossCharacter->GetActorLocation() - PlayerCharacter->GetActorLocation()).Size();

	GetBlackboardComponent()->SetValueAsBool(TEXT("IsStunning"), BossCharacter->IsStunning());
	
	UE_LOG(LogTemp, Log, TEXT("Boss AI %f %f %f"), Distance, BossCharacter->GetMeleeRange(), BossCharacter->GetMidRange())
	if (Distance < BossCharacter->GetMeleeRange())
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsCloseRange"), true);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsMidRange"), false);
	}
	else if (Distance < BossCharacter->GetMidRange())
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsMidRange"), true);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsCloseRange"), false);
	}
	else
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsCloseRange"), false);
		GetBlackboardComponent()->SetValueAsBool(TEXT("IsMidRange"), false);
	}
}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	BossCharacter = Cast<ABossCharacter>(InPawn);
}

void ABossAIController::OnUnPossess()
{
	Super::OnUnPossess();
	Destroy();
}
