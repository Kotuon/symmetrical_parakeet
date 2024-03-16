// Fill out your copyright notice in the Description page of Project Settings.

#include "Fall.h"
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class
#include "Components/CapsuleComponent.h"              // UCapsuleComponent
#include "Kismet/KismetMathLibrary.h"                 // RadiansToDegrees ()

// Sets default values for this component's properties
UFall::UFall() {
    PrimaryComponentTick.bCanEverTick = true;
    // ...
}

// Called when the game starts
void UFall::BeginPlay() {
    Super::BeginPlay();
    // ...

    parent = Cast< APlayerCharacter >( GetOwner() );
    character_movement = parent->GetCharacterMovement();
    capsule = parent->GetCapsuleComponent();

    world = GetWorld();

    parent->LandedDelegate.AddUniqueDynamic( this, &UFall::OnLanded );
    parent->MovementModeChangedDelegate.AddUniqueDynamic( this, &UFall::MovementModeChanged );
}

// Called every frame
void UFall::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
    // ...

    const FVector last_input = parent->GetLastMovementInput();

    if ( falling ) {
        Movement( last_input );
    }

    if ( character_movement->IsFalling() ) {
        air_time += DeltaTime;

        if ( !falling ) {
            const FVector position = parent->GetActorLocation();
            const FVector end = position + FVector( 0.f, 0.f, -min_fall_height );

            FHitResult hit_result;
            bool over_land = GetWorld()->LineTraceSingleByChannel( hit_result, position, end, ECollisionChannel::ECC_Visibility );
            if ( !over_land ) {
                DrawDebugLine( GetWorld(), position, end, FColor::Green, false, 2.f );
                Toggle( true );
            } else {
                DrawDebugLine( GetWorld(), position, end, FColor::Red, false, 2.f );
                falling = false;
            }
        }
    }
}

void UFall::MovementModeChanged( ACharacter *Character, EMovementMode PrevMovementMode, uint8 PrevCustomMode ) {
    if ( PrevMovementMode == MOVE_Falling && parent->GetCharacterMovement()->MovementMode == MOVE_Flying ) {
        air_time = 0.f;
        if ( falling ) {
            Toggle( false );
        }
    }
}

// Diving jump
bool UFall::CheckDivingJump() {
    const FVector position = parent->GetActorLocation();
    const FVector fwd = parent->GetActorForwardVector();

    const FVector start = position + ( 400.f * fwd ) + FVector( 0.f, 0.f, 200.f );
    const FVector end = position + ( 400.f * fwd ) + FVector( 0.f, 0.f, -2000.f );

    FHitResult hit_result;
    const bool result = GetWorld()->LineTraceSingleByChannel( hit_result, start, end, ECollisionChannel::ECC_Visibility );

    // DrawDebugLine( GetWorld(), start, end, FColor::Green, false, 5.f, ( uint8 )0U, 2.f );
    // GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Green, result ? "Glide: True" : "Glide: False" );

    diving_jump = !result;

    return diving_jump;
}

float UFall::GetAirTime() const {
    return air_time;
}

// Rotation
float UFall::GetPitchSpeed() const {
    return current_pitch_speed;
}

float UFall::GetRollSpeed() const {
    return current_roll_speed;
}

// Landing
void UFall::OnLanded( const FHitResult &Hit ) {
    going_to_land = false;

    if ( air_time > time_for_hard_landing ) {
        if ( IsValid( land_hard_animation ) )
            parent->PlayAnimMontage( land_hard_animation );
    }

    air_time = 0.f;
    if ( falling ) {
        Toggle( false );
    }
}

bool UFall::GetGoingToLand() const {
    return going_to_land;
}

float UFall::GetDistanceFromGround() const {
    return distance_from_ground;
}

void UFall::Toggle( bool should_enable ) {
    if ( should_enable ) {
        falling = true;
        capsule->SetCapsuleHalfHeight( capsule->GetScaledCapsuleHalfHeight() / 2.f, true );
    } else {
        falling = false;
        capsule->SetCapsuleHalfHeight( capsule->GetScaledCapsuleHalfHeight() * 2.f, true );
        parent->SetActorRotation( FRotator( 0., parent->GetActorRotation().Yaw, 0.f ) );

        current_pitch_speed = 0.f;
        current_roll_speed = 0.f;
    }
}

bool UFall::IsFalling() const {
    return falling;
}

void UFall::Movement( const FVector &last_input ) {
    const float delta = world->GetDeltaSeconds();

    const float roll = parent->GetActorRotation().Roll;

    const float roll_rot = current_roll_speed;
    const float pitch_rot = current_pitch_speed;
    const float yaw_rot = current_roll_speed;

    const FRotator new_rotation( pitch_rot, yaw_rot, roll_rot );
    parent->AddActorWorldRotation( new_rotation * delta );

    ForwardBackwardMovement( last_input );
    LeftRightMovement( last_input );
}

void UFall::ForwardBackwardMovement( const FVector &last_input ) {
    const float pitch = parent->GetActorRotation().Pitch;

    const float pitch_a = ( -1.f * last_input.Y * pitch_turn_speed ) + ( abs( current_roll_speed ) * -0.2f );
    const float pitch_b = pitch + 6.0 * -0.3;
    bool pitch_check = false;

    if ( ( last_input.Y < 0.f && pitch < 70.f ) || ( last_input.Y > 0.f && pitch > -70.f ) ) {
        pitch_check = true;
    }

    // if ( last_input.Y > 0.f ) {
    // const FVector fwd = parent->GetActorForwardVector();

    // parent->AddMovementInput( fwd, fall_fwd_speed, false );
    // FVector result = character_movement->Velocity;
    // if ( character_movement->Velocity.SizeSquared2D() > FMath::Square( max_fall_fwd_speed ) ) {
    //     result = FVector::PointPlaneProject( result, FVector::ZeroVector, fwd ) + fwd * max_fall_fwd_speed;
    //     character_movement->Velocity = result;
    // }
    // }

    if ( last_input.Y < 0.f ) {
        FVector result = character_movement->Velocity;
        const FVector grav_direction = character_movement->GetGravityDirection();
        if ( result.Z < 0.f ) {
            if ( result.SizeSquared() > FMath::Square( max_pull_back_speed ) ) {
                result = FVector::PointPlaneProject( result, FVector::ZeroVector, grav_direction ) + grav_direction * max_pull_back_speed;
                character_movement->Velocity = result;
            }
        }
    }

    const float new_pitch_speed = pitch_check ? pitch_a : pitch_b;

    current_pitch_speed = FMath::FInterpTo( current_pitch_speed, new_pitch_speed, world->GetDeltaSeconds(), 3.f );
}

void UFall::LeftRightMovement( const FVector &last_input ) {
    const float roll = parent->GetActorRotation().Roll;

    const float roll_a = last_input.X * roll_turn_speed;
    const float roll_b = roll * -0.5f;

    bool roll_check = false;

    if ( ( last_input.X > 0.f && roll < 50.f ) || ( last_input.X < 0.f && roll > -50.f ) ) {
        if ( last_input.X != 0.f ) {
            roll_check = true;
        }
    }

    const float new_roll_speed = roll_check ? roll_a : roll_b;
    current_roll_speed = FMath::FInterpTo( current_roll_speed, new_roll_speed, world->GetDeltaSeconds(), 2.f );
}
