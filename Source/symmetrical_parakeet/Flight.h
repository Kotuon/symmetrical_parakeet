// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Curves/CurveVector.h"
#include "Flight.generated.h"

class UCharacterMovementComponent;

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SYMMETRICAL_PARAKEET_API UFlight : public UAction {
    GENERATED_BODY()

protected: // Functions
    virtual void BeginPlay() override;

public: // Functions
    UFlight();
    virtual void Start( const FInputActionValue &value );
    virtual void End();

    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) override;

public: // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
    UCurveVector *velocity_curve;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
    float total_curve_time = 1.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
    float flight_power = 500.f;

private: // Variables
    UCharacterMovementComponent *character_movement;
    float time_held = 0.f;
    bool is_running = false;
};
