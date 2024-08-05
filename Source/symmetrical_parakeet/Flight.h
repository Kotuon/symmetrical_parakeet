// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Curves/CurveVector.h"
#include "Flight.generated.h"

class UCharacterMovementComponent;
class UProgressBar;

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

    UFUNCTION( BlueprintCallable )
    bool IsRunning() const;

    UFUNCTION( BlueprintCallable )
    float GetTimeHeld() const;

    UFUNCTION( BlueprintCallable )
    float GetAnimationTimeHeld() const;

private: // Functions
    void StartBarAlpha();
    void UpdateBarAlpha();

    float CheckSide( FVector player_position, FVector target_position, FVector range_side );

public: // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Curve" )
    UCurveFloat *velocity_curve;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Curve" )
    float total_curve_time = 1.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Speed" )
    float max_speed = 1000.f;

private: // Variables
    UCharacterMovementComponent *character_movement;
    FTimerHandle resource_bar_timer;
    FTimerHandle time_held_timer;

    UProgressBar *resource_bar;

    float curr_speed;

    float bar_alpha;

    float time_held = 0.f;
    float animation_time_held = 0.f;

    bool is_running = false;
};
