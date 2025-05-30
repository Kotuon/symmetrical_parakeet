// Fill out your copyright notice in the Description page of Project Settings.

#include "Jump.h"
#include "ActionManager.h"                            // UActionManager class
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class
#include "Components/CapsuleComponent.h"              // UCapsuleComponent class
#include "Kismet/KismetMathLibrary.h"                 // MapRangeClamped
#include "Glide.h"                                    // UGlide class
#include "GlideFunctions.h"                           // Glide interface
#include "Fall.h"                                     // UFall class

UJump::UJump() {
    type = EAction::A_Jump;
}

void UJump::BeginPlay() {
    Super::BeginPlay();

    character_movement = parent->GetCharacterMovement();

    parent->MovementModeChangedDelegate.AddUniqueDynamic( this, &UJump::MovementModeChanged );
    parent->LandedDelegate.AddUniqueDynamic( this, &UJump::OnLanded );
}

void UJump::Start( const FInputActionValue &value ) {
    if ( value.Get< bool >() ) {
        parent->SetLastMovementZInput( 1.f );
    } else {
        parent->SetLastMovementZInput( 0.f );
        End();
        return;
    }

    if ( character_movement->IsFalling() && !jump_memory ) {
        jump_memory = true;
        parent->GetWorldTimerManager().SetTimer( jump_memory_handle, this, &UJump::ResetJumpMemory, 0.1f, false, jump_memory_time );
        return;
    }
    has_jumped = true;

    if ( !manager->StartAction( type ) ) {
        return;
    }

    JumpTakeOff();
}

void UJump::End() {
    Super::End();

    parent->StopJumping();
}

void UJump::JumpTakeOff() {
    const FVector velocity = character_movement->Velocity;
    const float velocity_xy = velocity.Size2D();

    character_movement->JumpZVelocity = UKismetMathLibrary::MapRangeClamped( velocity_xy, 400.f, 800.f, 700.f, 900.f );

    // TODO change glide to fall
    const bool result = parent->fall->CheckDivingJump();

    UAnimInstance *anim = parent->GetMesh()->GetAnimInstance();

    if ( anim->Implements< UGlideFunctions >() ) {
        IGlideFunctions::Execute_SetDiving( anim, result );
    }

    parent->Jump();
    character_movement->bNotifyApex = true;
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

void UJump::SetHasPlayedAnimation() {
    has_played_jump_animation = true;
}

bool UJump::GetHasPlayedAnimation() const {
    return has_played_jump_animation;
}

// Landing
void UJump::OnLanded( const FHitResult &Hit ) {
    has_played_jump_animation = false;
}
