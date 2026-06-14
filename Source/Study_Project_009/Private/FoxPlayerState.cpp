#include "FoxPlayerState.h"

void AFoxPlayerState::IncrementTryCount()
{
	CurrentTryCount++;
}

void AFoxPlayerState::ResetTryCount()
{
	CurrentTryCount = 0;
}

int32 AFoxPlayerState::GetCurrentTryCount() const
{
	return CurrentTryCount;
}

int32 AFoxPlayerState::GetMaxTryCount() const
{
	return MaxTryCount;
}
