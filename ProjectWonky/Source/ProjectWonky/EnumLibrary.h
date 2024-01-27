#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEnemyStates : uint8
{
	ES_DEFAULT,
	ES_Attack,
	ES_Idle,
	ES_MoveToTarget,
	ES_ENTRY_AMOUNT
};