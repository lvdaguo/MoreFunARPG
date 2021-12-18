#pragma once

namespace PlayerCharacterController
{
	static const FName PrimaryAttack = TEXT("PrimaryAttack");
	static const FName Heal = TEXT("Heal");
	static const FName Running = TEXT("Running");
	static const FName Roll = TEXT("Roll");

	static const FName MoveForward = TEXT("MoveForward");
	static const FName MoveRight = TEXT("MoveRight");
	static const FName Turn = TEXT("Turn");
	static const FName LookUp = TEXT("LookUp");
}

namespace MonsterAIController
{
	static const FName IsPlayerInSight = TEXT("IsPlayerInSight");
	static const FName IsPlayerDead = TEXT("IsPlayerDead");
	static const FName MovingPosition = TEXT("MovingPosition");
	static const FName PlayerActor = TEXT("PlayerActor");
	static const FName IsLowHealth = TEXT("IsLowHealth");
	static const FName HasHealthPotion = TEXT("HasHealPotion");
}

namespace BossAIController
{
	static const FName PlayerActor = TEXT("PlayerActor");
	static const FName IsChargeReady = TEXT("IsChargeReady");
	static const FName IsPlayerDead = TEXT("IsPlayerDead");
	static const FName IsStunning = TEXT("IsStunning");
	static const FName IsCloseRange = TEXT("IsCloseRange");
	static const FName IsMidRange = TEXT("IsMidRange");
}