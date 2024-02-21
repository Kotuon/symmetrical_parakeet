// Fill out your copyright notice in the Description page of Project Settings.

#include "Sprint.h"
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class

USprint::USprint() {
    type = EAction::A_Sprint;
}

void USprint::BeginPlay() {
    Super::BeginPlay();

    character_movement = parent->GetCharacterMovement();

    original_walk_speed = character_movement->MaxWalkSpeed;
}

void USprint::Start( const FInputActionValue &value ) {
    if ( value.Get< bool >() ) {
        is_sprinting = true;
        character_movement->MaxWalkSpeed = sprint_speed;
    } else {
        is_sprinting = false;
        character_movement->MaxWalkSpeed = original_walk_speed;
    }
}

void USprint::End() {
    Super::End();
}