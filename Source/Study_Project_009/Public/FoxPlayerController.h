#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FoxPlayerController.generated.h"

UCLASS()
class STUDY_PROJECT_009_API AFoxPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Exec)
	void SubmitGuess(const FString& GuessString);
	
	UFUNCTION(Client, Reliable)
	void Client_ShowGameResult(const FString& WinnerName, bool bIsDraw, const FString& CorrectAnswer);
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Chat")
	void Server_SendChatMessage(const FString& Message);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddChatMessage(const FString& SenderName, const FString & Message);
	
protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SubmitGuess(const FString& GuessString);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnShowGameResult(const FString& WinnerName, bool bIsDraw, const FString& CorrectAnswer);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void OnReceiveChatMessage(const FString& SenderName, const FString& Message);
	
private:
	bool IsValidInput(const FString& InputString) const;
	
};
