// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "Jump.generated.h"

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
    // Jump
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Animation" )
    UAnimMontage *standing_jump_animation;
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Animation" )
    UAnimMontage *running_jump_animation;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "General" )
    float jump_velocity = 420.f;

    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "General|Jump Memory" )
    float jump_memory_time = 0.25f;

private: // Variables
    FTimerHandle jump_memory_handle;

    float start_jump_velocity;
    bool has_jumped = false;
    bool jump_memory = false;
};
