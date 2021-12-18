#include "Controller/BossAIController.h"

#include "Controller/MonsterAIController.h"
#include "Spawner.h"
#include "ARPGCharacter/BossCharacter.h"
#include "ARPGCharacter/PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "GlobalNameText.h"
#include "MacroAIHelper.h"

// Constructor
ABossAIController::ABossAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Setup Runtime
void ABossAIController::SetupDelegate()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	PlayerDieHandle = PlayerCharacter->PlayerDieEvent().AddLambda([this]()
	{
		BB_SET_BOOL(BossAIController::IsPlayerDead, true)
	});

	ASpawner* Spawner = Cast<ASpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	PlayerRespawnHandle = Spawner->PlayerRespawnEvent().AddLambda([this]()
	{
		BB_SET_BOOL(BossAIController::IsPlayerDead, false)
	});
}

// Life Cycle
void ABossAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviourTree);

	BB_SET_BOOL(BossAIController::IsPlayerDead, false)
	SetupDelegate();

	AActor* PlayerCharacter = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	BB_SET_OBJECT(BossAIController::PlayerActor, PlayerCharacter)
	BB_SET_BOOL(BossAIController::IsChargeReady, true)
}

void ABossAIController::Tick(const float DeltaSeconds)
{
	using namespace BossAIController;
	Super::Tick(DeltaSeconds);

	const AActor* PlayerCharacter = Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(PlayerActor));
	const float Distance = (BossCharacter->GetActorLocation() - PlayerCharacter->GetActorLocation()).Size();

	BB_SET_BOOL(IsStunning, BossCharacter->IsStunning())

	if (Distance < BossCharacter->GetMeleeRange())
	{
		BB_SET_BOOL(IsCloseRange, true)
		BB_SET_BOOL(IsMidRange, false)
	}
	else if (Distance < BossCharacter->GetMidRange())
	{
		BB_SET_BOOL(IsCloseRange, false)
		BB_SET_BOOL(IsMidRange, true)
	}
	else
	{
		BB_SET_BOOL(IsCloseRange, false)
		BB_SET_BOOL(IsMidRange, false)
	}
}

// Task Helper
void ABossAIController::StartChargeCoolDownCountDown()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		BB_SET_BOOL(BossAIController::IsChargeReady, true)
	}), BossCharacter->GetChargeCoolDownTime(), false);
}

void ABossAIController::RemoveListener() const
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	ASpawner* Spawner = Cast<ASpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawner::StaticClass()));
	PlayerCharacter->PlayerDieEvent().Remove(PlayerDieHandle);
	Spawner->PlayerRespawnEvent().Remove(PlayerRespawnHandle);
}

// Override
void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	BossCharacter = Cast<ABossCharacter>(InPawn);
}

void ABossAIController::OnUnPossess()
{
	Super::OnUnPossess();
	RemoveListener();
	Destroy();
}
