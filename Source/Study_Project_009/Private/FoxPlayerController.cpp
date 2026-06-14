#include "FoxPlayerController.h"

#include "FoxGameModeBase.h"
#include "FoxPlayerState.h"

void AFoxPlayerController::SubmitGuess(const FString& GuessString)
{
	// 이미 기회를 다 썼는지 체크
	AFoxPlayerState* FoxState = GetPlayerState<AFoxPlayerState>();
	if (FoxState && FoxState->GetCurrentTryCount() >= FoxState->GetMaxTryCount())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("이미 기회를 모두 소진하였습니다!"));
		}
		return;
	}

	if (!IsValidInput(GuessString))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("다시 입력하세요 (중복 없는 3자리 숫자)"));
		}
		return;
	}
	Server_SubmitGuess(GuessString);
}

void AFoxPlayerController::Client_ShowGameResult_Implementation(const FString& WinnerName, bool bIsDraw, const FString& CorrectAnswer)
{
	OnShowGameResult(WinnerName, bIsDraw, CorrectAnswer);
}

bool AFoxPlayerController::Server_SubmitGuess_Validate(const FString& GuessString)
{
	AFoxGameModeBase* GameMode = Cast<AFoxGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		return GameMode->IsValidInput(GuessString);
	}
	return false;
}

void AFoxPlayerController::Server_SubmitGuess_Implementation(const FString& GuessString)
{
	AFoxGameModeBase* GameMode = Cast<AFoxGameModeBase>(GetWorld()->GetAuthGameMode());
	AFoxPlayerState* FoxState = GetPlayerState<AFoxPlayerState>();
	
	if (GameMode && FoxState)
	{
		// 시도 횟수 증가
		FoxState->IncrementTryCount();
		
		int32 Current = FoxState->GetCurrentTryCount();
		int32 Max = FoxState->GetMaxTryCount();
		
		// 화면 및 로그에 시도 횟수 출력 [현재 / 최대]
		FString TryInfo = FString::Printf(TEXT("[%d / %d]"), Current, Max);
		
		int32 Strikes = 0;
		int32 Balls = 0;
		FString Result = GameMode->CalculateResult(GuessString, Strikes, Balls);
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("시도: %s - 결과: %s"), *TryInfo, *Result));
		}
		
		UE_LOG(LogTemp, Log, TEXT("플레이어 %s 입력: %s -> 판정: %s %s"), *GetName(), *GuessString, *Result, *TryInfo);
		
		// ★ [추가] 심판에게 게임 결과를 체크하도록 요청합니다!
		GameMode->CheckGameResult(this, Strikes);
	}
}

bool AFoxPlayerController::IsValidInput(const FString& InputString) const
{
	if (InputString.Len() != 3) return false;
	
	TSet<TCHAR> UniqueChars;
	for (TCHAR Char : InputString)
	{
		if (!FChar::IsDigit(Char) || Char == '0') return false;
		UniqueChars.Add(Char);
	}
	return UniqueChars.Num() == 3;
}

void AFoxPlayerController::Server_SendChatMessage_Implementation(const FString& Message)
{
	AFoxGameModeBase* GameMode = Cast<AFoxGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GameMode && GameMode->IsValidInput(Message))
	{
		Server_SubmitGuess(Message);
	}
	else
	{
		FString SenderName = PlayerState ? PlayerState->GetPlayerName() : GetName();

		int32 HyphenIndex;
		if (SenderName.FindChar('-', HyphenIndex))
		{
			SenderName = SenderName.Left(HyphenIndex);
		}
		Multicast_AddChatMessage(SenderName, Message);
	}
}

void AFoxPlayerController::Multicast_AddChatMessage_Implementation(const FString& SenderName, const FString& Message)
{
	OnReceiveChatMessage(SenderName, Message);
}
