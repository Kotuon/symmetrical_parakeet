// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Curves/CurveVector.h"
#include "Flight.generated.h"

class UCharacterMovementComponent;
class UProgressBar;
class USpline;

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

    bool IsRunning() const;

private: // Functions
    void StartBarAlpha();
    void UpdateBarAlpha();
    void StartRecharge();

public: // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Spline"  )
    USplineComponent *spline;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Curve"  )
    UCurveFloat *velocity_curve;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Curve"  )
    float total_curve_time = 1.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Speed"  )
    float flight_power = 1000.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Speed"  )
    float max_start_speed = 500.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Resource"  )
    float total_flight_time = 5.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Resource"  )
    float recharge_rate = 0.2f;

private: // Variables
    UCharacterMovementComponent *character_movement;
    FTimerHandle resource_bar_timer;
    FTimerHandle time_held_timer;

    UProgressBar *resource_bar;

    float bar_alpha;

    float time_held = 0.f;
    float curr_time_held = 0.f;

    bool is_running = false;
    bool can_recharge = false;
};
