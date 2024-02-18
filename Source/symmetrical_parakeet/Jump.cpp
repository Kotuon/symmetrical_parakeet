// Fill out your copyright notice in the Description page of Project Settings.

#include "Jump.h"
#include "ActionManager.h"                            // UActionManager class
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class
#include "Components/CapsuleComponent.h"              // UCapsuleComponent class

UJump::UJump() {
    type = EAction::A_Jump;
}

void UJump::BeginPlay() {
    Super::BeginPlay();

    start_jump_velocity = jump_velocity;
    parent->MovementModeChangedDelegate.AddUniqueDynamic( this, &UJump::MovementModeChanged );
}

void UJump::Start( const FInputActionValue &value ) {
    if ( value.Get< bool >() ) {
        parent->SetLastMovementZInput( 1.f );
    } else {
        parent->SetLastMovementZInput( 0.f );
    }

    if ( has_jumped ) {
        jump_memory = true;
        parent->GetWorldTimerManager().SetTimer( jump_memory_handle, this, &UJump::ResetJumpMemory, 0.1f, false, jump_memory_time );
        return;
    }
    has_jumped = true;

    start_jump_velocity = parent->GetVelocity().Length();

    if ( !manager->StartAction( type ) ) {
        return;
    }

    if ( start_jump_velocity > 0 ) {
        if ( IsValid( running_jump_animation ) ) {
            parent->PlayAnimMontage( running_jump_animation );
        } else {
            JumpTakeOff();
        }
        return;
    }

    if ( IsValid( standing_jump_animation ) ) {
        parent->PlayAnimMontage( standing_jump_animation );
    } else {
        JumpTakeOff();
    }
}

void UJump::End() {
    Super::End();
}

void UJump::JumpTakeOff() {
    const FVector up_direction = parent->GetCapsuleComponent()->GetUpVector();
    FVector xy_direction = FVector( 0 );

    const FVector last_movement_input = parent->GetLastMovementInput();

    xy_direction = ( parent->gimbal->GetForwardVector() * last_movement_input.Y ) + ( parent->gimbal->GetRightVector() * last_movement_input.X );
    xy_direction.Normalize();

    FVector jump_direction = ( up_direction * jump_velocity ) + ( xy_direction * start_jump_velocity );

    parent->LaunchCharacter( jump_direction, true, true );
}

void UJump::MovementModeChanged( ACharacter *Character, EMovementMode PrevMovementMode, uint8 PrevCustomMode ) {
    if ( PrevMovementMode == MOVE_Falling && parent->GetCharacterMovement()->MovementMode == MOVE_Walking ) {
        has_jumped = false;
        End();
        if ( jump_memory )
            Start( FInputActionValue() );
    }
}

void UJump::ResetJumpMemory() {
    jump_memory = false;
}
