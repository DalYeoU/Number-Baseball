#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FoxGameState.generated.h"

UCLASS()
class STUDY_PROJECT_009_API AFoxGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BroadcastChat(const FString& Message);
};