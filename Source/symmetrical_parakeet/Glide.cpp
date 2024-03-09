// Fill out your copyright notice in the Description page of Project Settings.

#include "Glide.h"
#include "PlayerCharacter.h"                          // APlayerCharacter class
#include "GlideFunctions.h"                           // Glide interface
#include "Components/CapsuleComponent.h"              // UCapsuleComponent
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class
#include "Kismet/KismetMathLibrary.h"                 // NormalizeToRange()

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
    capsule = parent->GetCapsuleComponent();

    parent->LandedDelegate.AddUniqueDynamic( this, &UGlide::OnLanded );
    parent->OnReachedJumpApex.AddUniqueDynamic( this, &UGlide::OnJumpApexReached );
}

// Called every frame
void UGlide::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
    // ...

    const FVector last_input = parent->GetLastMovementInput();

    // Falling( last_input );
    GlideTick( DeltaTime, last_input );
    Movement( last_input, DeltaTime );

    if ( character_movement->IsFalling() ) {
        air_time += DeltaTime;

        if ( !gliding ) {
            const FVector position = parent->GetActorLocation();
            const FVector end = position + FVector( 0.f, 0.f, -min_fall_height );

            FHitResult hit_result;
            bool over_land = GetWorld()->LineTraceSingleByChannel( hit_result, position, end, ECollisionChannel::ECC_Visibility );
            if ( !over_land ) {
                DrawDebugLine( GetWorld(), position, end, FColor::Green, false, 2.f );
                Toggle( true );
            } else {
                DrawDebugLine( GetWorld(), position, end, FColor::Red, false, 2.f );
            }
        }
    }
}

void UGlide::OnLanded( const FHitResult &Hit ) {
    has_lift = false;
    going_to_land = false;
    character_movement->bOrientRotationToMovement = true;

    if ( air_time > time_for_hard_landing ) {
        if ( IsValid( land_hard_animation ) )
            parent->PlayAnimMontage( land_hard_animation );
    }

    air_time = 0.f;
}

void UGlide::OnJumpApexReached() {
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

    const FVector start = position + ( 400.f * fwd ) + FVector( 0.f, 0.f, 200.f );
    const FVector end = position + ( 400.f * fwd ) + FVector( 0.f, 0.f, -2000.f );

    FHitResult hit_result;
    const bool result = GetWorld()->LineTraceSingleByChannel( hit_result, start, end, ECollisionChannel::ECC_Visibility );

    // DrawDebugLine( GetWorld(), start, end, FColor::Green, false, 5.f, ( uint8 )0U, 2.f );
    // GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Green, result ? "Glide: True" : "Glide: False" );

    diving_jump = !result;

    return diving_jump;
}

void UGlide::Falling( const FVector &last_input ) {
    current_fwd_speed = 0.f;

    if ( has_default_falling_lift && has_lift ) {
        const FVector last_velocity_cmp = parent->GetVelocity();

        if ( last_input.Y > 0.f ) {
            return;
        }

        if ( character_movement->IsFalling() ) {
            const bool pull_up = last_input.Y < 0.f;
            const float lift_value = default_falling_lift * ( pull_up ? pull_up_modifier : 1.f );
            const float lift_value_neg = -1.f * lift_value;

            if ( last_velocity_cmp.Z != lift_value_neg ) {
                current_velocity = FVector( 0.f, 0.f, lift_value );
                parent->LaunchCharacter( current_velocity, false, false );
            }

            const FVector position = parent->GetActorLocation();
            const FVector end = position + FVector( 0.f, 0.f, -landing_distance );

            FHitResult hit_result;
            going_to_land = GetWorld()->LineTraceSingleByChannel( hit_result, position, end, ECollisionChannel::ECC_Visibility );
            if ( going_to_land ) {
                DrawDebugLine( GetWorld(), position, end, FColor::Red );
                distance_from_ground = FVector::Dist( position, hit_result.ImpactPoint );
            }
        }
    }
}

void UGlide::Movement( const FVector &last_input, float DeltaTime ) {
    axis.X = last_input.X;
    axis.Y = last_input.Y;

    ForwardBackwardMovement( DeltaTime );
    LeftRightMovement( DeltaTime );
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

float UGlide::GetDistanceFromGround() const {
    return distance_from_ground;
}

bool UGlide::GetGoingToLand() const {
    return going_to_land;
}

bool UGlide::GetHasLift() const {
    return has_lift;
}

void UGlide::Toggle( bool should_enable ) {
    if ( should_enable ) {
        if ( gliding ) {
            return;
        }

        gliding = true;

        // TODO: Play start/stop sound effect

        capsule->SetSimulatePhysics( true );
        capsule->SetCapsuleHalfHeight( capsule->GetScaledCapsuleHalfHeight() / 2.f, true );

        StartGliding();

        glide_velocity = parent->GetActorRotation().UnrotateVector( capsule->GetComponentVelocity() );
        character_movement->AddImpulse( glide_velocity * initial_impulse_strength, true );

        GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Yellow, "Start gliding." );

        // TODO: Add particle effect
    } else {
        gliding = false;

        // TODO: Play start/stop sound effect

        capsule->SetSimulatePhysics( false );
        capsule->SetCapsuleHalfHeight( capsule->GetScaledCapsuleHalfHeight() * 2.f, true );
        parent->SetActorRotation( FRotator( 0., parent->GetActorRotation().Yaw, 0.f ) );

        air_time = 0.f;
        current_pitch_speed = 0.f;
        current_roll_speed = 0.f;

        character_movement->Velocity = FVector::Zero();

        axis.X = 0.f;
        axis.Y = 0.f;

        GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Yellow, "Zero velocity." );

        last_velocity = FVector::Zero();
        current_velocity = FVector::Zero();
        glide_velocity = FVector::Zero();
    }
}

void UGlide::StartGliding() {
    current_fwd_speed = 0.f;
    current_pitch_speed = 0.f;
    current_roll_speed = 0.f;

    const float upper_momentum = parent->GetVelocity().Length() * 1.2f;
    fwd_momentum = FMath::Clamp( upper_momentum, 0.f, upper_momentum );
    current_fwd_speed = 0.f;

    capsule->SetPhysicsLinearVelocity( UKismetMathLibrary::GetForwardVector( parent->GetActorRotation() ) * initial_velocity );

    timeout = 2.f;
}

void UGlide::GlideTick( float DeltaTime, const FVector &last_input ) {
    const FVector unrotated_velocity = parent->GetActorRotation().UnrotateVector( capsule->GetComponentVelocity() );
    const float unclamped_fwd_speed = unrotated_velocity.X;
    current_fwd_speed = FMath::Clamp( unclamped_fwd_speed, 0.f, unclamped_fwd_speed );

    // TODO: Update effects and sound based on velocity

    if ( !gliding ) {
        Falling( last_input );
        return;
    }

    // TODO: Hit Traces
    const bool result = HitTraces();
    if ( result ) {
        Toggle( false );
        GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Green, "Stop gliding." );
    } else {
        UpdateGliding( DeltaTime );
    }
}

bool UGlide::HitTraces() {
    bool result = false;

    FHitResult hit_result;
    FVector start = parent->GetActorLocation();
    FVector end = start - FVector( 0.f, 0.f, down_trace_distance );

    result = GetWorld()->LineTraceSingleByChannel( hit_result, start, end, ECollisionChannel::ECC_Visibility );
    if ( result ) {
        return true;
    }

    end = start + FVector( 0.f, 0.f, down_trace_distance );
    result = GetWorld()->LineTraceSingleByChannel( hit_result, start, end, ECollisionChannel::ECC_Visibility );
    if ( result ) {
        return true;
    }

    start = parent->GetActorLocation() + FVector( 0.f, 0.f, trace_height_offset );
    end = start + ( parent->GetActorForwardVector() * front_trace_distance );
    result = GetWorld()->LineTraceSingleByChannel( hit_result, start, end, ECollisionChannel::ECC_Visibility );
    if ( result ) {
        return true;
    }

    end += ( parent->GetActorRightVector() * front_angled_trace_distance );
    result = GetWorld()->LineTraceSingleByChannel( hit_result, start, end, ECollisionChannel::ECC_Visibility );
    if ( result ) {
        return true;
    }

    end -= 2.f * ( parent->GetActorRightVector() * front_angled_trace_distance );
    result = GetWorld()->LineTraceSingleByChannel( hit_result, start, end, ECollisionChannel::ECC_Visibility );
    if ( result ) {
        return true;
    }

    return false;
}

void UGlide::UpdateGliding( float DeltaTime ) {
    if ( timeout > 0.f ) {
        axis = FVector( 0.f );
        timeout -= 0.1f;
    } else {
        const float curve_time = FMath::Clamp( parent->GetActorRotation().Pitch, -80.f, 80.f );
        const float curve_time_normalized = UKismetMathLibrary::NormalizeToRange( curve_time, -80.f, 80.f );
        const float scaled_curve_time = DeltaTime * acceleration * pitch_curve->GetFloatValue( curve_time_normalized );
        const float unclamped_value = scaled_curve_time + fwd_momentum;
        fwd_momentum = FMath::Clamp( unclamped_value, max_momentum * -1.f, max_momentum );
    }

    const FVector unrotated_momentum = parent->GetActorRotation().UnrotateVector( FVector( fwd_momentum, 0.f, 0.f ) );
    const float lerp_alpha = UKismetMathLibrary::NormalizeToRange( current_fwd_speed, 0.f, 1200.f );
    const float lerp_alpha_clamped = FMath::Clamp( lerp_alpha, 0.f, 0.95f );
    const FVector new_vel = FMath::Lerp( capsule->GetComponentVelocity(), unrotated_momentum, lerp_alpha_clamped );

    const FVector final_vel = FMath::VInterpTo( capsule->GetComponentVelocity(), new_vel, DeltaTime, 5.f );
    capsule->SetPhysicsLinearVelocity( final_vel );

    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Red, final_vel.ToString() );
    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Green, character_movement->Velocity.ToString() );
    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Yellow, parent->GetActorLocation().ToString() );

    last_velocity = capsule->GetComponentVelocity();

    const float roll_rot = current_roll_speed * DeltaTime;
    const float pitch_rot = current_pitch_speed * DeltaTime;
    const float yaw_rot = parent->GetActorRotation().Roll * DeltaTime;
    const FRotator new_rotation = FRotator( pitch_rot, yaw_rot, roll_rot );
    parent->AddActorLocalRotation( new_rotation );
}

float UGlide::GetPitchSpeed() const {
    return current_pitch_speed;
}

float UGlide::GetRollSpeed() const {
    return current_roll_speed;
}
