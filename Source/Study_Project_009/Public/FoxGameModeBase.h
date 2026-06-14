#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FoxGameModeBase.generated.h"

UCLASS()
class STUDY_PROJECT_009_API AFoxGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	TArray<int32> AnswerNumbers;
	void GenerateAnswerNumbers();
	AFoxGameModeBase();
	FTimerHandle ResetTimerHandle;
	
public:
	bool IsValidInput(const FString& InputString) const;
	void CheckGameResult(class AFoxPlayerController* PlayerController, int32 Strikes);
	void ResetGame();
	
	FString CalculateResult(const FString& InputString, int32& OutStrikes, int32& OutBalls);
	
private:
	void HandleWin(class AFoxPlayerController* Winner, const FString& CorrectAnswer);
	void HandleDraw(const FString& CorrectAnswer);
	bool HasAnyTryLeft() const;
	
	void CountStrikesAndBalls(const FString& InputString, int32& OutStrikes, int32& OutBalls) const;
	FString FormatResultString(int32 Strikes, int32 Balls) const;
};
