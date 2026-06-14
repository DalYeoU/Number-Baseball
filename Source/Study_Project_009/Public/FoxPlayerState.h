#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FoxPlayerState.generated.h"

UCLASS()
class STUDY_PROJECT_009_API AFoxPlayerState : public APlayerState
{
	GENERATED_BODY()
	
private:
	int32 CurrentTryCount = 0;
	int32 MaxTryCount = 3;
	
public:
	void IncrementTryCount();
	void ResetTryCount();
	
	int32 GetCurrentTryCount() const;
	int32 GetMaxTryCount() const;
};
