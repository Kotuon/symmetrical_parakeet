// Fill out your copyright notice in the Description page of Project Settings.

#include "Glide.h"
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GlideFunctions.h"                           // Glide interface
#include "Components/CapsuleComponent.h"              // UCapsuleComponent
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class

// Sets default values for this component's properties
UGlide::UGlide() {
    PrimaryComponentTick.bCanEverTick = true;
    // ...
}

// Called when the game starts
void UGlide::BeginPlay() {
    Super::BeginPlay();
    // ...

    parent = Cast< APlayerCharacter >( GetOwner() );
    character_movement = parent->GetCharacterMovement();

    parent->LandedDelegate.AddUniqueDynamic( this, &UGlide::OnLanded );
    parent->OnReachedJumpApex.AddUniqueDynamic( this, &UGlide::OnJumpApexReached );
}

// Called every frame
void UGlide::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
    // ...

    const FVector last_input = parent->GetLastMovementInput();

    Falling( last_input );
    Movement( last_input, DeltaTime );

    air_time += DeltaTime;
}

void UGlide::OnLanded( const FHitResult &Hit ) {
    has_lift = false;

    is_diving = false;
    UAnimInstance *anim = parent->GetMesh()->GetAnimInstance();

    IGlideFunctions *i_anim = Cast< IGlideFunctions >( anim );
    if ( i_anim ) {
        i_anim->Execute_Dive( anim, false );
    }

    if ( air_time > time_for_hard_landing ) {
        if ( IsValid( land_hard_animation ) )
            parent->PlayAnimMontage( land_hard_animation );
    }

    air_time = 0.f;
}

void UGlide::OnJumpApexReached() {
    has_lift = true;
}

float UGlide::GetAirTime() const {
    return air_time;
}

float UGlide::GetTimeForHardLanding() const {
    return time_for_hard_landing;
}

bool UGlide::CheckDivingJump() {
    const FVector position = parent->GetActorLocation();
    const FVector fwd = parent->GetActorForwardVector();

    const FVector start = position + ( 200.f * fwd ) + FVector( 0.f, 0.f, 200.f );
    const FVector end = position + ( 200.f * fwd ) + FVector( 0.f, 0.f, -2000.f );

    FHitResult hit_result;
    const bool result = GetWorld()->LineTraceSingleByChannel( hit_result, start, end, ECollisionChannel::ECC_Visibility );

    diving_jump = !result;

    return diving_jump;
}

void UGlide::Dive_Implementation( bool is_diving_ ) {
    is_diving = is_diving_;

    UAnimInstance *anim = parent->GetMesh()->GetAnimInstance();

    IGlideFunctions *i_anim = Cast< IGlideFunctions >( anim );
    if ( i_anim ) {
        i_anim->Execute_Dive( anim, is_diving );
    }
}

void UGlide::Falling( const FVector &last_input ) {
    if ( has_default_falling_lift && has_lift ) {
        const FVector last_velocity = parent->GetVelocity();

        if ( last_input.X + last_input.Y == 0.f ) {
            if ( character_movement->IsFalling() ) {
                if ( !is_diving ) {
                    const float lift_value = default_falling_lift * -1.f;

                    if ( last_velocity.Y != lift_value ) {
                        current_velocity = FVector( 0.f, 0.f, lift_value );
                        parent->LaunchCharacter( current_velocity, false, false );
                    }
                }
            }
        }
    }
}

void UGlide::Movement( const FVector &last_input, float DeltaTime ) {
    if ( !is_diving ) {
        axis.Y = last_input.Y;

    } else {
        axis.X = 0.f;
        axis.Y = 1.f;
    }
}

void UGlide::ForwardBackwardMovement( float DeltaTime ) {
    const FRotator p_rotation = parent->GetActorRotation();

    const float pitch_a = ( -1.f * axis.Y * pitch_turn_speed ) + ( abs( current_roll_speed ) * -0.2f );
    const float pitch_b = p_rotation.Pitch + 6.0 * -0.3;
    bool pitch_check = false;

    if ( ( axis.Y < 0.f && p_rotation.Pitch < 70.f ) || ( axis.Y > 0.f && p_rotation.Pitch > -70.f ) ) {
        if ( axis.Y != 0.f ) {
            pitch_check = true;
        }
    }

    const float new_pitch_speed = pitch_check ? pitch_a : pitch_b;

    current_pitch_speed = FMath::FInterpTo( current_pitch_speed, new_pitch_speed, DeltaTime, 2.f );
}

void UGlide::LeftRightMovement( float DeltaTime ) {
    const FRotator p_rotation = parent->GetActorRotation();

    const float roll_a = axis.X * roll_turn_speed;
    const float roll_b = p_rotation.Roll * -0.5f;

    bool roll_check = false;

    if ( ( axis.X > 0.f && p_rotation.Roll < 50.f ) || ( axis.X < 0.f && p_rotation.Roll > -50.f ) ) {
        if ( axis.X != 0.f ) {
            roll_check = true;
        }
    }

    const float new_roll_speed = roll_check ? roll_a : roll_b;
    current_roll_speed = FMath::FInterpTo( current_roll_speed, new_roll_speed, DeltaTime, 2.f );
}
