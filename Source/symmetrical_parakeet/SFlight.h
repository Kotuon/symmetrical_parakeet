// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "SFlight.generated.h"

class USplineComponent;

UCLASS()
class SYMMETRICAL_PARAKEET_API USFlight : public UAction {
    GENERATED_BODY()
protected: // Functions
    virtual void BeginPlay() override;

public: // Functions
    USFlight();
    virtual void Start( const FInputActionValue &value );
    virtual void End();

    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) override;

    bool IsRunning() const;

private: // Functions
public:  // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Spline" )
    USplineComponent *spline;

private: // Variables
    bool is_running = false;
};
