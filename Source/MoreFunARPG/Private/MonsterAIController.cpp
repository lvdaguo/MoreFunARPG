#include "MonsterAIController.h"

#include "MonsterCharacter.h"
#include "PlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"

AMonsterAIController::AMonsterAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviourTree);

	MonsterCharacter = Cast<AMonsterCharacter>(GetPawn());
	check(MonsterCharacter != nullptr)

	static const FName PlayerActor(TEXT("PlayerActor"));
	GetBlackboardComponent()->SetValueAsObject(PlayerActor,
	                                           GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AMonsterAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	static const FName IsLowHealth(TEXT("IsLowHealth"));
	static const FName HasHealthPotion(TEXT("HasHealPotion"));

	const float Cur = static_cast<float>(MonsterCharacter->GetCurHealth());
	const float Max = static_cast<float>(MonsterCharacter->GetMaxHealth());
	const float HealthPercent = Cur / Max;

	GetBlackboardComponent()->SetValueAsBool(IsLowHealth,
	                                         HealthPercent <= MonsterCharacter->GetLowHealthPercent());
	GetBlackboardComponent()->SetValueAsBool(HasHealthPotion,
	                                         MonsterCharacter->GetHealthPotion() > 0);
}

void AMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, const FAIStimulus& InStimulus)
{
	static const FName IsPlayerInSight(TEXT("IsPlayerInSight"));

	GetBlackboardComponent()->SetValueAsBool(IsPlayerInSight,
	                                         InStimulus.WasSuccessfullySensed());
}
