// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "ABGameMode.h"
#include "Player/ABPlayerController.h"
#include "ArenaBattle.h"
#include "ABGameState.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "ABPlayerState.h"

AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/BP_ABCharacterPlayer.BP_ABCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ArenaBattle.ABPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	GameStateClass = AABGameState::StaticClass();
	PlayerStateClass = AABPlayerState::StaticClass();
}

FTransform AABGameMode::GetRandomStartTransform() const
{
	// 없을 수도 있음
	if (PlayerStartArray.Num() == 0)
	{
		return FTransform(FVector(0.0f, 0.0f, 230.0f));
	}

	int32 RandIndex = FMath::RandRange(0, PlayerStartArray.Num() - 1);
	return PlayerStartArray[RandIndex]->GetActorTransform();
}

void AABGameMode::OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	APlayerState* KillerPlayerState = Killer->PlayerState;
	if (KillerPlayerState)
	{
		KillerPlayerState->SetScore(KillerPlayerState->GetScore() + 1);

		if (KillerPlayerState->GetScore() > 2)
		{
			FinishMatch();
		}
	}
}

//void AABGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
//{
//	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("============================================================"));
//	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
//
//	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
//	//ErrorMessage = TEXT("Server Is Full");
//
//	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
//}
//
//APlayerController* AABGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
//{
//	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
//
//	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
//
//	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
//
//	return NewPlayerController;
//}
//
//void AABGameMode::PostLogin(APlayerController* NewPlayer)
//{
//	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
//
//	Super::PostLogin(NewPlayer);
//
//	UNetDriver* NetDriver = GetNetDriver();
//	if (NetDriver)
//	{
//		for (const auto& Connection : NetDriver->ClientConnections)
//		{
//			AB_LOG(LogABNetwork, Log, TEXT("Client Connections : %s"), *Connection->GetName());
//		}
//	}
//	else
//	{
//		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No NetDriver"));
//	}
//
//	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
//}

void AABGameMode::StartPlay()
{
	Super::StartPlay();

	for (APlayerStart* PlayerStart : TActorRange<APlayerStart>(GetWorld()))
	{
		PlayerStartArray.Add(PlayerStart);
	}
}

void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 초당 호출
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AABGameMode::DefaultGameTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void AABGameMode::DefaultGameTimer()
{
	AABGameState* const ABGameState = Cast<AABGameState>(GameState);

	if (ABGameState && ABGameState->RemainingTime > 0)
	{
		ABGameState->RemainingTime--;
		AB_LOG(LogABNetwork, Log, TEXT("Remaining Time : %d"), ABGameState->RemainingTime);
		if (ABGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::InProgress)
			{
				// 진행중이면 게임 종료
				FinishMatch();
			}
			else if (GetMatchState() == MatchState::WaitingPostMatch)
			{
				// 결과화면 대기중이면 다른 월드로 이동
				GetWorld()->ServerTravel(TEXT("/Game/ArenaBattle/Maps/Part3Step2?listen"));
			}
		}
	}
}

void AABGameMode::FinishMatch()
{
	AABGameState* const ABGameState = Cast<AABGameState>(GameState);
	if (ABGameState && IsMatchInProgress())
	{
		EndMatch(); // 매치 상태를 WaitingPostMatch로 변경
		ABGameState->RemainingTime = ABGameState->ShowResultWaitingTime;
	}
}
