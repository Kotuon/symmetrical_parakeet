#pragma once

#include "CoreMinimal.h"
#include "ActionTypes.generated.h"

UENUM( BlueprintType )
enum class EAction : uint8 {
    A_None UMETA( DisplayName = "None" ),
    A_Climb UMETA( DisplayName = "Climb" ),
    A_Dash UMETA( DisplayName = "Dash" ),
    A_WallStick UMETA( DisplayName = "WallStick" ),
    A_Jump UMETA( DisplayName = "Jump" ),
    A_WallRun UMETA( DisplayName = "WallRun" )
};