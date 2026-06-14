#include "FoxGameModeBase.h"

#include "FoxPlayerController.h"
#include "FoxPlayerState.h"
#include "GameFramework/GameStateBase.h"

void AFoxGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 정답 생성 함수 호출
	GenerateAnswerNumbers();
}

void AFoxGameModeBase::GenerateAnswerNumbers()
{
	/// 1~9단계 배열 만들기
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; i++)
	{
		Numbers.Add(i);
	}
	
	/// 셔플
	for (int32 i = Numbers.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		Numbers.Swap(i, j);
	}
	
	/// 앞의 3개만 저장
	AnswerNumbers.Empty();
	for (int32 i = 0; i < 3; i++)
	{
		AnswerNumbers.Add(Numbers[i]);
	}
	
	/// 디버깅용 로그 출력
	UE_LOG(LogTemp, Log, TEXT("AnswerNumbers: %d %d %d"), AnswerNumbers[0], AnswerNumbers[1], AnswerNumbers[2]);
}

AFoxGameModeBase::AFoxGameModeBase()
{
	PlayerControllerClass = AFoxPlayerController::StaticClass();
	PlayerStateClass = AFoxPlayerState::StaticClass();
}

bool AFoxGameModeBase::IsValidInput(const FString& InputString) const
{
	/// 조건 1: 3자리 숫자인가?
	if (InputString.Len() != 3)
	{
		UE_LOG(LogTemp, Error, TEXT("InputString is not 3 digits"));
		return false;
	}
	
	/// 조건 2 & 3: 문자가 포함되었는지 및 중복 숫자가 있는지 검사
	TSet<TCHAR> UniqueChars;
	for (TCHAR Char : InputString)
	{
		/// 숫자가 아닌 문자가 들어있다면 false
		if (!FChar::IsDigit(Char) || Char == '0')
		{
			return false;
		}
		UniqueChars.Add(Char);
	}
	
	/// 중복된 숫자가 없다면 UniqueChars의 크기는 3이어야 합니다.
	return UniqueChars.Num() == 3;
}

FString AFoxGameModeBase::CalculateResult(const FString& InputString, int32& OutStrikes, int32& OutBalls)
{
	CountStrikesAndBalls(InputString, OutStrikes, OutBalls);
	return FormatResultString(OutStrikes, OutBalls);
}

void AFoxGameModeBase::CountStrikesAndBalls(const FString& InputString, int32& OutStrikes, int32& OutBalls) const
{
	OutStrikes = 0;
	OutBalls = 0;

	for (int32 i = 0; i < 3; i++)
	{
		int32 InputDigit = InputString[i] - '0';

		if (InputDigit == AnswerNumbers[i])
		{
			OutStrikes++;
		}
		else if (AnswerNumbers.Contains(InputDigit))
		{
			OutBalls++;
		}
	}
}

FString AFoxGameModeBase::FormatResultString(int32 Strikes, int32 Balls) const
{
	if (Strikes == 0 && Balls == 0)
	{
		return TEXT("OUT");
	}

	FString ResultStr = TEXT("");
	if (Strikes > 0)
	{
		ResultStr += FString::Printf(TEXT("%dS"), Strikes);
	}
	if (Balls > 0)
	{
		ResultStr += FString::Printf(TEXT("%dB"), Balls);
	}

	return ResultStr;
}

void AFoxGameModeBase::CheckGameResult(class AFoxPlayerController* PlayerController, int32 Strikes)
{
	if (!PlayerController) return;

	// 정답 3자리를 문자열로 조립합니다 (예: "786")
	FString CorrectStr = FString::Printf(TEXT("%d%d%d"), AnswerNumbers[0], AnswerNumbers[1], AnswerNumbers[2]);
	
	/// 1. 승리 판정: 누군가 3개의 스트라이크를 다 맞췄다면?
	if (Strikes == 3)
	{
		HandleWin(PlayerController, CorrectStr);
		return;
	}
	
	/// 2. 무승부 판정: 모든 플레이어가 기회를 다 썼고 아무도 남은 기회가 없다면?
	if (!HasAnyTryLeft())
	{
		HandleDraw(CorrectStr);
	}
}

void AFoxGameModeBase::HandleWin(class AFoxPlayerController* Winner, const FString& CorrectAnswer)
{
	if (!Winner) return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("WINNER: %s!"), *Winner->GetName()));
	}
	UE_LOG(LogTemp, Log, TEXT("WINNER: %s!"), *Winner->GetName());
	
	if (GameState)
	{
		// PlayerState로부터 실제 유저 이름을 획득합니다 (널 체크 포함)
		FString WinnerName = Winner->PlayerState ? Winner->PlayerState->GetPlayerName() : Winner->GetName();

		for (APlayerState* PS : GameState->PlayerArray)
		{
			AFoxPlayerController* PC = Cast<AFoxPlayerController>(PS->GetOwner());
			if (PC)
			{
				PC->Client_ShowGameResult(WinnerName, false, CorrectAnswer);
			}
		}
	}
	
	/// 게임을 즉시 리셋하고 종료
	ResetGame();
}

void AFoxGameModeBase::HandleDraw(const FString& CorrectAnswer)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("GAME OVER!"));
	}
	UE_LOG(LogTemp, Log, TEXT("GAME OVER!"));
	
	if (GameState)
	{
		for (APlayerState* PS : GameState->PlayerArray)
		{
			AFoxPlayerController* PC = Cast<AFoxPlayerController>(PS->GetOwner());
			if (PC)
			{
				PC->Client_ShowGameResult(TEXT("Draw"), true, CorrectAnswer);
			}
		}
	}
	
	ResetGame();
}

bool AFoxGameModeBase::HasAnyTryLeft() const
{
	if (!GameState) return false;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		AFoxPlayerState* State = Cast<AFoxPlayerState>(PS);
		if (State && State->GetCurrentTryCount() < State->GetMaxTryCount())
		{
			return true;
		}
	}
	return false;
}

void AFoxGameModeBase::ResetGame()
{
	/// 1. 모든 플레이어의 시도 횟수를 0으로 리셋
	if (GameState)
	{
		for (APlayerState* PS : GameState->PlayerArray)
		{
			AFoxPlayerState* State = Cast<AFoxPlayerState>(PS);
			if (State)
			{
				State->ResetTryCount();
			}
		}
	}

	/// 2. 새로운 정답 생성
	GenerateAnswerNumbers();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("게임을 초기화하고 새 게임을 시작합니다!"));
	}
}