// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GlideFunctions.generated.h"

// This class does not need to be modified.
UINTERFACE( MinimalAPI, BlueprintType, Blueprintable )
class UGlideFunctions : public UInterface {
    GENERATED_BODY()
};

/**
 *
 */
class SYMMETRICAL_PARAKEET_API IGlideFunctions {
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UFUNCTION( BlueprintCallable, BlueprintImplementableEvent )
    void SetDiving( bool is_diving_ );
};
