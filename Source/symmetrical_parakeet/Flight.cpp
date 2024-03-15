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
    if ( !value.Get< bool >() || time_held >= total_flight_time ) {
        End();
        return;
    }

    if ( !is_running ) {
        if ( !manager->StartAction( type ) ) {
            return;
        }
        parent->GetWorldTimerManager().ClearTimer( resource_bar_timer );
        resource_bar->SetRenderOpacity( 1.f );

        is_running = true;
        can_recharge = false;

        if ( character_movement->MovementMode != MOVE_Falling ) {
            const FVector up_velocity = ( parent->gimbal->GetUpVector() ) * ( 100.f );
            character_movement->AddImpulse( up_velocity, true );
        } else {
            FVector result = character_movement->Velocity;
            const FVector grav_direction = character_movement->GetGravityDirection();
            if ( result.SizeSquared() > FMath::Square( 100.f ) ) {
                result = FVector::PointPlaneProject( result, FVector::ZeroVector, grav_direction ) + grav_direction * 1000.f;
                character_movement->Velocity = result;
            }
        }
        character_movement->SetMovementMode( MOVE_Flying );
    }

    const float curve_value = velocity_curve->GetFloatValue( FMath::Clamp( time_held / total_curve_time, 0.f, 1.f ) );
    const float scale_value = flight_power * curve_value;

    const FVector fwd = parent->gimbal->GetForwardVector();
    const FVector up = parent->gimbal->GetUpVector();
    // const FVector fwd = parent->GetActorForwardVector();
    // const FVector up = parent->GetActorUpVector();

    const float alpha = FMath::Clamp( curr_time_held / total_curve_time, 0.f, 1.f );
    const FVector flight_direction = FMath::CubicInterp( fwd, fwd, up, up, alpha );

    character_movement->AddImpulse( flight_direction * scale_value, true );

    DrawDebugLine( GetWorld(), parent->GetActorLocation(), parent->GetActorLocation() + ( flight_direction * scale_value ), FColor::Green, false, 0.f, ( uint8 )0U, 10.f );

    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Cyan, flight_direction.ToString() );
}

void UFlight::End() {
    Super::End();

    is_running = false;
    character_movement->SetMovementMode( MOVE_Falling );

    curr_time_held = 0.f;

    parent->GetWorldTimerManager().SetTimer( resource_bar_timer, this, &UFlight::StartBarAlpha, 5.f, false );
    parent->GetWorldTimerManager().SetTimer( time_held_timer, this, &UFlight::StartRecharge, 2.f, false );
}

void UFlight::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if ( !is_running && can_recharge ) {
        time_held = FMath::Clamp( time_held - DeltaTime, 0.f, total_flight_time );
    } else if ( is_running ) {
        time_held = FMath::Clamp( time_held + DeltaTime, 0.f, total_flight_time );
        curr_time_held = FMath::Clamp( curr_time_held + DeltaTime, 0.f, total_curve_time );
    }

    resource_bar->SetPercent( ( total_flight_time - time_held ) / total_flight_time );

    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Red, "Time held: " + FString::SanitizeFloat( time_held ) );
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

void UFlight::StartRecharge() {
    can_recharge = true;
}
