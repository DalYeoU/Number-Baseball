#include "FoxGameState.h"

void AFoxGameState::Multicast_BroadcastChat_Implementation(const FString& Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Message);
	}
}
