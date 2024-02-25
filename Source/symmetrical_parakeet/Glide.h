// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlideFunctions.h"
#include "Glide.generated.h"

class APlayerCharacter;
class UCharacterMovementComponent;

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

    UFUNCTION( BlueprintCallable, BlueprintNativeEvent, Category = "Glide" )
    void Dive( bool is_diving_ );
    virtual void Dive_Implementation( bool is_diving_ ) override;

    UFUNCTION( BlueprintCallable )
    float GetDistanceFromGround() const;

    UFUNCTION( BlueprintCallable )
    bool GetGoingToLand() const;

private: // Functions
    void Falling( const FVector &last_input );
    void Movement( const FVector &last_input, float DeltaTime );
    void ForwardBackwardMovement( float DeltaTime );
    void LeftRightMovement( float DeltaTime );

public: // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Animations" )
    UAnimMontage *land_hard_animation;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Animations" )
    float time_for_hard_landing = 2.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Default Lift" )
    bool has_default_falling_lift = true;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Default Lift" )
    float default_falling_lift = 8.f;

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

private: // Variables
    APlayerCharacter *parent;
    UCharacterMovementComponent *character_movement;

    FVector current_velocity;
    FVector axis;

    float distance_from_ground;

    float current_pitch_speed;
    float current_roll_speed;

    float current_fwd_speed = 0.f;
    float air_time = 0.f;

    bool going_to_land = false;

    bool has_lift = false;

    bool diving_jump = false;
    bool is_diving = false;
};
