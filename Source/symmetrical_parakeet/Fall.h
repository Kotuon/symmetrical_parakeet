// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Fall.generated.h"

class APlayerCharacter;
class UCharacterMovementComponent;
class UCapsuleComponent;

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SYMMETRICAL_PARAKEET_API UFall : public UActorComponent {
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public: // Functions
    UFall();

    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) override;

    UFUNCTION()
    void MovementModeChanged( ACharacter *Character, EMovementMode PrevMovementMode, uint8 PrevCustomMode );

    // Diving Jump
    UFUNCTION( BlueprintCallable )
    bool CheckDivingJump();

    UFUNCTION( BlueprintCallable )
    float GetAirTime() const;

    // Rotation
    UFUNCTION( BlueprintCallable )
    float GetPitchSpeed() const;
    UFUNCTION( BlueprintCallable )
    float GetRollSpeed() const;

    // Landing
    UFUNCTION()
    void OnLanded( const FHitResult &Hit );
    UFUNCTION( BlueprintCallable )
    bool GetGoingToLand() const;
    UFUNCTION( BlueprintCallable )
    float GetDistanceFromGround() const;

    UFUNCTION( BlueprintCallable )
    bool IsFalling() const;

private: // Functions
    void Toggle( bool should_enable );

    // Movement
    void Movement( const FVector &last_input );
    void ForwardBackwardMovement( const FVector &last_input );
    void LeftRightMovement( const FVector &last_input );

public: // Variables
    // Fall distance
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float landing_distance = 500.f;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float min_fall_height = 100.f;

    // Landing
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Animations" )
    UAnimMontage *land_hard_animation;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Animations" )
    float time_for_hard_landing = 2.f;

    // Rotation
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    UCurveFloat *pitch_curve;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float pitch_turn_speed = 100.f;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float roll_turn_speed = 80.f;

    // In air movement
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float max_pull_back_speed = 3000.f;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float fall_fwd_speed = 25.f;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Settings" )
    float max_fall_fwd_speed = 100.f;

private: // Variables
    APlayerCharacter *parent;
    UCharacterMovementComponent *character_movement;
    UCapsuleComponent *capsule;

    UWorld *world;

    float air_time = 0.f;

    // Rotation
    float current_pitch_speed;
    float current_roll_speed;

    // Landing
    float distance_from_ground;
    bool going_to_land = false;

    // Diving Jump
    bool diving_jump = false;

    bool falling = false;
};
