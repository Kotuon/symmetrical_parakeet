// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class USmartSpringArm;
class UCameraComponent;
class UAction;
class UActionManager;

UCLASS()
class SYMMETRICAL_PARAKEET_API APlayerCharacter : public ACharacter {
    GENERATED_BODY()

protected: // Functions
    virtual void BeginPlay() override;

public: // Functions
    APlayerCharacter( const FObjectInitializer &ObjectInitializer );

    virtual void Tick( float DeltaTime ) override;

    virtual void SetupPlayerInputComponent( class UInputComponent *PlayerInputComponent ) override;

    void StartPlayerRotation();

    UFUNCTION( BlueprintCallable )
    void SetCanWalk( bool canWalk );

    UFUNCTION( BlueprintCallable )
    const FVector2D &GetLastMovementInput() const;

public: // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Actions" )
    UActionManager *action_manager;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Input" )
    class UInputMappingContext *input_mapping;

    // Input Actions
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions" )
    UInputAction *input_move;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Actions" )
    UInputAction *input_look;

    // Camera components
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Camera" )
    USceneComponent *gimbal;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Camera" )
    USmartSpringArm *spring_arm;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Camera" )
    UCameraComponent *camera;

    // Running
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Jump" )
    UAnimMontage *run_to_stop_animation;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Camera" )
    float sensitivity = 1.f;

private: // Functions
    void Move( const FInputActionValue &value );
    void Look( const FInputActionValue &value );

    void UpdatePlayerRotation();

    void UpdateCameraRotation();

private: // Variables
    FTimerHandle camera_rotation_time_handle;
    FRotator new_camera_rotation;
    FRotator old_camera_rotation;

    FTimerHandle player_rotation_time_handle;
    FRotator new_player_rotation;
    FRotator old_player_rotation;

    FVector2D last_movement_input;

    float timeSinceRotateStarted = 0.f;

    bool can_walk = true;
};
