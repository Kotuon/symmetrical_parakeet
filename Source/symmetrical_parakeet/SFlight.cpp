// Fill out your copyright notice in the Description page of Project Settings.

#include "SFlight.h"
#include "Components/SplineComponent.h" // USplineComponent class
#include "PlayerCharacter.h"            // APlayerCharacter class

USFlight::USFlight() {
    type = EAction::A_SFlight;
    PrimaryComponentTick.bCanEverTick = true;
    SetComponentTickEnabled( true );

    spline = Cast< USplineComponent >( CreateDefaultSubobject< USplineComponent >( FName( "FlightSpline" ) ) );
}

void USFlight::BeginPlay() {
    Super::BeginPlay();
    //...

    spline->ClearSplinePoints( true );
    spline->AddPoint( FSplinePoint( 0.f, parent->GetActorLocation() ), false );
    spline->AddPoint( FSplinePoint( 0.5f, parent->GetActorLocation() ), false );
    spline->AddPoint( FSplinePoint( 1.f, parent->GetActorLocation() ), false );
    spline->UpdateSpline();
}

void USFlight::Start( const FInputActionValue &value ) {
    const bool input = value.Get< bool >();

    if ( !input ) {
        End();
        return;
    }

    if ( is_running ) {
        return;
    }

    // TODO: Start flying with spline
    is_running = true;

    const FVector position = parent->GetActorLocation();
    const FVector fwd = parent->GetActorForwardVector();

    TArray< FVector > points = {
        position,
        position + ( fwd * 500.f ),
        position + ( fwd * 1000.f ),
    };

    spline->SetSplinePoints( points, ESplineCoordinateSpace::Local, true );
}

void USFlight::End() {
    Super::End();

    is_running = false;
}

void USFlight::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if ( !is_running ) {
        return;
    }

    const FVector last_input = parent->GetLastMovementInput();

    // TODO: Update last spline point
    const FVector position = parent->GetActorLocation();
    const FVector fwd = parent->GetActorForwardVector();
    const FVector up = parent->GetActorUpVector();

    FVector new_position = position + ( fwd * 1000.f );
    if ( abs( last_input.X ) > 0.f ) {
        const FVector right = parent->GetActorRightVector();
        new_position += ( right * ( last_input.X * 200.f ) );
    }
    if ( abs( last_input.Y ) > 0.f ) {
        const FVector right = parent->GetActorRightVector();
        new_position += ( up * ( last_input.Y * 200.f ) );
    }
    spline->SetLocationAtSplinePoint( 2, new_position, ESplineCoordinateSpace::Local, false );

    spline->UpdateSpline();

    // TODO: Update player position
}

bool USFlight::IsRunning() const {
    return is_running;
}
