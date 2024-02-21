// Fill out your copyright notice in the Description page of Project Settings.

#include "Jump.h"
#include "ActionManager.h"                            // UActionManager class
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class
#include "Components/CapsuleComponent.h"              // UCapsuleComponent class
#include "Kismet/KismetMathLibrary.h"                 // MapRangeClamped
#include "Glide.h"                                    // UGlide class
#include "GlideFunctions.h"                           // Glide interface

UJump::UJump() {
    type = EAction::A_Jump;
}

void UJump::BeginPlay() {
    Super::BeginPlay();

    character_movement = parent->GetCharacterMovement();

    parent->MovementModeChangedDelegate.AddUniqueDynamic( this, &UJump::MovementModeChanged );
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
    const FVector velocity = parent->GetVelocity();
    const float velocity_xy = velocity.Size2D();

    character_movement->JumpZVelocity = UKismetMathLibrary::MapRangeClamped( velocity_xy, 400.f, 800.f, 500.f, 700.f );

    const bool result = parent->glide->CheckDivingJump();

    UAnimInstance *anim = parent->GetMesh()->GetAnimInstance();

    IGlideFunctions *i_anim = Cast< IGlideFunctions >( anim );
    if ( i_anim ) {
        i_anim->Execute_SetDiving( anim, result );
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
