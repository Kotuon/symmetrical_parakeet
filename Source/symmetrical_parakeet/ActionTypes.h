#pragma once

#include "CoreMinimal.h"
#include "ActionTypes.generated.h"

UENUM( BlueprintType )
enum class EAction : uint8 {
    A_None UMETA( DisplayName = "None" ),
    A_Sprint UMETA( DisplayName = "Sprint" ),
    A_Jump UMETA( DisplayName = "Jump" ),
    A_Flight UMETA( DisplayName = "Flight" ),
    A_SFlight UMETA( DisplayName = "SFlight" ),
    A_GravityRush UMETA( DisplayName = "GravityRush" )
};