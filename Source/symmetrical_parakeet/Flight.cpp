// Fill out your copyright notice in the Description page of Project Settings.

#include "Flight.h"
#include "ActionManager.h"                            // UActionManager class
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class
#include "Camera/CameraComponent.h"                   // UCameraComponent class
#include "Blueprint/UserWidget.h"                     // UUserWidget class
#include "Components/WidgetComponent.h"               // UWidgetComponent class
#include "Components/ProgressBar.h"                   // UProgressBar
#include "Components/Widget.h"                        // UWidget class
#include "Math/UnrealMathUtility.h"                   // FMath

UFlight::UFlight() {
    type = EAction::A_Flight;
    PrimaryComponentTick.bCanEverTick = true;
    SetComponentTickEnabled( true );
}

void UFlight::BeginPlay() {
    Super::BeginPlay();

    character_movement = parent->GetCharacterMovement();

    TArray< UWidgetComponent * > widgets;
    parent->GetComponents< UWidgetComponent >( widgets );

    for ( UWidgetComponent *widget : widgets ) {
        GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Green, widget->GetName() );
        if ( widget->GetName() == "ResourceBar" ) {
            widget->InitWidget();
            UUserWidget *user_widget = widget->GetWidget();
            if ( IsValid( user_widget ) ) {
                resource_bar = Cast< UProgressBar >( user_widget->GetWidgetFromName( "ResourceBar" ) );
                resource_bar->SetRenderOpacity( 0.f );
            } else {
                GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Green, "Bad Widget." );
            }
            break;
        }
    }
}

void UFlight::Start( const FInputActionValue &value ) {
    if ( !value.Get< bool >() ) {
        End();
        return;
    }

    if ( !is_running ) {
        if ( !manager->StartAction( type ) ) {
            return;
        }
        is_running = true;
        parent->SetCanWalk( false );

        if ( character_movement->MovementMode != MOVE_Falling ) {
            const FVector up_velocity = ( parent->gimbal->GetUpVector() ) * ( 100.f );
            character_movement->AddImpulse( up_velocity, true );
        } else {
            // FVector result = character_movement->Velocity;
            // const FVector grav_direction = character_movement->GetGravityDirection();
            // if ( result.SizeSquared() > FMath::Square( 100.f ) ) {
            //     result = FVector::PointPlaneProject( result, FVector::ZeroVector, grav_direction ) + grav_direction * 1000.f;
            //     character_movement->Velocity = result;
            // }
        }
        character_movement->SetMovementMode( MOVE_Flying );
        curr_speed = character_movement->Velocity.Length();
    }
}

void UFlight::End() {
    Super::End();
    //...

    is_running = false;
    parent->SetCanWalk( true );

    character_movement->SetMovementMode( MOVE_Falling );

    time_held = 0.f;
}

void UFlight::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if ( is_running ) {
        time_held += DeltaTime;
        animation_time_held = FMath::Clamp( animation_time_held + DeltaTime, 0.f, 1.f );

        const FVector last_input = parent->GetLastMovementInput();
        const FVector fwd = parent->camera->GetForwardVector();

        const float curve_value = velocity_curve->GetFloatValue( FMath::Clamp( time_held / total_curve_time, 0.f, 1.f ) );

        curr_speed = max_speed * curve_value;
        const FVector new_velocity = FMath::VInterpTo( character_movement->Velocity, fwd * curr_speed, DeltaTime, 1.f );
        character_movement->Velocity = new_velocity;

        const FQuat new_rotation = FMath::QInterpTo( parent->GetActorRotation().Quaternion(), character_movement->Velocity.Rotation().Quaternion(), DeltaTime, 5.f );

        parent->SetActorRotation( new_rotation );
    } else {
        animation_time_held = FMath::Clamp( animation_time_held - ( 2.f * DeltaTime ), 0.f, 1.f );
    }
}

void UFlight::StartBarAlpha() {
    bar_alpha = 1.f;
    parent->GetWorldTimerManager().SetTimer( resource_bar_timer, this, &UFlight::UpdateBarAlpha, 0.01f, true );
}

void UFlight::UpdateBarAlpha() {
    bar_alpha -= 0.01f;
    resource_bar->SetRenderOpacity( FMath::Clamp( bar_alpha, 0.f, 1.f ) );

    if ( bar_alpha < 0.f ) {
        parent->GetWorldTimerManager().ClearTimer( resource_bar_timer );
    }
}

bool UFlight::IsRunning() const {
    return is_running;
}

float UFlight::GetTimeHeld() const {
    return time_held;
}

float UFlight::GetAnimationTimeHeld() const {
    return animation_time_held;
}
