// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Jump.generated.h"

class UCharacterMovementComponent;

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SYMMETRICAL_PARAKEET_API UJump : public UAction {
    GENERATED_BODY()

protected: // Functions
    virtual void BeginPlay() override;

public: // Functions
    UJump();
    virtual void Start( const FInputActionValue &value );
    virtual void End();

    UFUNCTION( BlueprintCallable )
    void JumpTakeOff();

    UFUNCTION()
    void MovementModeChanged( ACharacter *Character, EMovementMode PrevMovementMode, uint8 PrevCustomMode );

    UFUNCTION()
    void ResetJumpMemory();

public: // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "General|Jump Memory" )
    float jump_memory_time = 0.25f;

private: // Variables
    UCharacterMovementComponent *character_movement;

    FTimerHandle jump_memory_handle;

    bool has_jumped = false;
    bool jump_memory = false;
};
