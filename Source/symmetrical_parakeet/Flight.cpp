// Fill out your copyright notice in the Description page of Project Settings.

#include "Flight.h"
#include "ActionManager.h"                            // UActionManager class
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class

UFlight::UFlight() {
    type = EAction::A_Flight;
    PrimaryComponentTick.bCanEverTick = true;
    SetComponentTickEnabled( true );
}

void UFlight::BeginPlay() {
    Super::BeginPlay();

    character_movement = parent->GetCharacterMovement();
}

void UFlight::Start( const FInputActionValue &value ) {
    if ( !value.Get< bool >() ) {
        End();
        return;
    }

    if ( !manager->StartAction( type ) ) {
        if ( !is_running ) {
            return;
        }
    }

    if ( !is_running ) {
        time_held = 0.f;
        is_running = true;
        character_movement->SetMovementMode( MOVE_Flying );
    }

    FVector new_force = parent->gimbal->GetForwardVector();//velocity_curve->GetVectorValue( FMath::Clamp( time_held / total_curve_time, 0.f, 1.f ) );
    character_movement->AddImpulse( new_force * flight_power, false );

    DrawDebugLine( GetWorld(), parent->GetActorLocation(), parent->GetActorLocation() + ( new_force * flight_power ), FColor::Green, false, 0.f, ( uint8 )0U, 5.f );

    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Green, new_force.ToString() );
}

void UFlight::End() {
    Super::End();

    is_running = false;
    character_movement->SetMovementMode( MOVE_Falling );
}

void UFlight::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if ( !is_running )
        return;

    time_held += DeltaTime;
    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Red, "Updating." );
}
