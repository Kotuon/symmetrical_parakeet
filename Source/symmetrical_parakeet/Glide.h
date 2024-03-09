// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlideFunctions.h"
#include "Glide.generated.h"

class APlayerCharacter;
class UCharacterMovementComponent;
class UCapsuleComponent;

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SYMMETRICAL_PARAKEET_API UGlide : public UActorComponent, public IGlideFunctions {
    GENERATED_BODY()

protected: // Functions
    virtual void BeginPlay() override;

public: // Functions
    UGlide();

    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) override;

    UFUNCTION()
    void OnLanded( const FHitResult &Hit );

    UFUNCTION()
    void OnJumpApexReached();

    UFUNCTION( BlueprintCallable )
    float GetAirTime() const;

    UFUNCTION( BlueprintCallable )
    float GetTimeForHardLanding() const;

    UFUNCTION( BlueprintCallable )
    bool CheckDivingJump();

    UFUNCTION( BlueprintCallable )
    float GetDistanceFromGround() const;

    UFUNCTION( BlueprintCallable )
    bool GetGoingToLand() const;

    UFUNCTION( BlueprintCallable )
    bool GetHasLift() const;

    UFUNCTION()
    void Toggle( bool should_enable );

    UFUNCTION( BlueprintCallable )
    float GetPitchSpeed() const;

    UFUNCTION( BlueprintCallable )
    float GetRollSpeed() const;

private: // Functions
    void Falling( const FVector &last_input );
    void Movement( const FVector &last_input, float DeltaTime );
    void ForwardBackwardMovement( float DeltaTime );
    void LeftRightMovement( float DeltaTime );
    void StartGliding();
    void GlideTick( float DeltaTime, const FVector &last_input );
    bool HitTraces();
    void UpdateGliding( float DeltaTime );

public: // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Animations" )
    UAnimMontage *land_hard_animation;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Animations" )
    float time_for_hard_landing = 2.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Default Lift" )
    bool has_default_falling_lift = true;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Default Lift" )
    float default_falling_lift = 8.f;

    // Initial
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Initial" )
    float initial_velocity = 500.f;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Initial" )
    float initial_impulse_strength = 800.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float acceleration = 400.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float max_momentum = 10000.f;

    // Pitch
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    UCurveFloat *pitch_curve;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float pitch_turn_speed = 100.f;

    // Roll
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float roll_turn_speed = 80.f;

    // Fall distance
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float landing_distance = 500.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float pull_up_modifier = 4.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float min_fall_height = 100.f;

    // Trace distances
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Trace Distances" )
    float down_trace_distance = 120.f;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Trace Distances" )
    float front_trace_distance = 150.f;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Trace Distances" )
    float front_angled_trace_distance = 120.f;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Trace Distances" )
    float trace_height_offset = 50.f;

private: // Variables
    APlayerCharacter *parent;
    UCharacterMovementComponent *character_movement;
    UCapsuleComponent *capsule;

    FVector axis;

    FVector current_velocity;
    FVector last_velocity;
    FVector glide_velocity;

    float distance_from_ground;

    float current_pitch_speed;
    float current_roll_speed;

    float current_fwd_speed = 0.f;
    float fwd_momentum = 0.f;

    float timeout = 0.f;

    float air_time = 0.f;

    bool going_to_land = false;
    bool gliding = false;

    bool has_lift = false;

    bool diving_jump = false;
};
