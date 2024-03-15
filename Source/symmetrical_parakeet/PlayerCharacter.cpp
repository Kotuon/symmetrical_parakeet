
#include "PlayerCharacter.h"
#include "SmartSpringArm.h"                           // USmartSpringArm class
#include "Camera/CameraComponent.h"                   // UCameraComponent class
#include "EnhancedInputSubsystems.h"                  // UEnhancedInputLocalPlayerSubsystem class
#include "EnhancedInputComponent.h"                   // UEnhancedInputComponent class
#include "Kismet/KismetMathLibrary.h"                 // GetForwardVector(), GetRightVector()
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class
#include "Action.h"                                   // UAction class
#include "ActionManager.h"                            // UActionManager class
#include "Glide.h"                                    // UGlide class
#include "GameFramework/MovementComponent.h"          //
#include "Fall.h"                                     // UFall class
#include "Flight.h"                                   // UFlight class
#include "SFlight.h"                                   // UFlight class

// Sets default values
APlayerCharacter::APlayerCharacter( const FObjectInitializer &ObjectInitializer ) : ACharacter( ObjectInitializer ) {
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    character_movement = GetCharacterMovement();

    action_manager = Cast< UActionManager >( CreateDefaultSubobject< UActionManager >( FName( "ActionManager" ) ) );

    gimbal = Cast< USceneComponent >( CreateDefaultSubobject< USceneComponent >( FName( "Gimbal" ) ) );
    gimbal->SetupAttachment( GetRootComponent() );
    spring_arm = Cast< USmartSpringArm >( CreateDefaultSubobject< USmartSpringArm >( FName( "SpringArm" ) ) );
    spring_arm->SetupAttachment( gimbal );
    camera = CreateDefaultSubobject< UCameraComponent >( FName( "Camera" ) );
    camera->SetupAttachment( spring_arm );

    fall = Cast< UFall >( CreateDefaultSubobject< UFall >( FName( "Fall" ) ) );
    flight = Cast< UFlight >( CreateDefaultSubobject< UFlight >( FName( "Flight" ) ) );
    sflight = Cast< USFlight >( CreateDefaultSubobject< USFlight >( FName( "SFlight" ) ) );
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay() {
    Super::BeginPlay();

    UWorld *world = GetWorld();

    APlayerController *pc = world->GetFirstPlayerController();
    if ( !pc )
        return;

    pc->bShowMouseCursor = false;
}

// Called every frame
void APlayerCharacter::Tick( float DeltaTime ) {
    Super::Tick( DeltaTime );

    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Green, character_movement->Velocity.ToString() );
    GEngine->AddOnScreenDebugMessage( -1, 0.f, FColor::Red, character_movement->GetCurrentAcceleration().ToString() );
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent( UInputComponent *PlayerInputComponent ) {
    // Get the player controller
    const APlayerController *pc = Cast< APlayerController >( GetController() );

    // Get the local player subsystem
    UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem >( pc->GetLocalPlayer() );
    // Clear out existing mapping, and add our mapping
    Subsystem->ClearAllMappings();
    Subsystem->AddMappingContext( input_mapping, 0 );

    // Get the EnhancedInputComponent
    UEnhancedInputComponent *PEI = Cast< UEnhancedInputComponent >( PlayerInputComponent );

    // Bind the actions
    PEI->BindAction( input_move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move );
    PEI->BindAction( input_look, ETriggerEvent::Triggered, this, &APlayerCharacter::Look );

    TArray< UAction * > action_components;
    GetComponents< UAction >( action_components );
    for ( UAction *action : action_components ) {
        if ( IsValid( action->input_action ) )
            PEI->BindAction( action->input_action, ETriggerEvent::Triggered, action, &UAction::Start );
    }
}

void APlayerCharacter::Move( const FInputActionValue &value ) {
    const FVector2D input_value = value.Get< FVector2D >();
    last_movement_input = FVector( input_value.X, input_value.Y, 0.f );

    if ( fall->IsFalling() || flight->IsRunning() || sflight->IsRunning() ) {
        return;
    }

    const FRotator gimbal_rotation_yaw = FRotator{ 0.0, gimbal->GetComponentRotation().Yaw, 0.0 };

    const FVector world_forward = UKismetMathLibrary::GetForwardVector( gimbal_rotation_yaw );
    const FVector world_right = UKismetMathLibrary::GetRightVector( gimbal_rotation_yaw );

    AddMovementInput( world_forward, input_value.Y, false );
    AddMovementInput( world_right, input_value.X, false );
}

void APlayerCharacter::Look( const FInputActionValue &value ) {
    const FVector2D input_value = value.Get< FVector2D >();

    // FRotator new_yaw = GetActorRightVector().Rotation() * input_value.X * sensitivity * 0.01f;
    const FRotator new_yaw{ 0.0, input_value.X * sensitivity, 0.0 };
    gimbal->AddWorldRotation( new_yaw );

    const float input_pitch = spring_arm->GetRelativeRotation().Pitch + ( input_value.Y * sensitivity );
    const float clamped_pitch = FMath::Clamp( input_pitch, -89, 90 );

    const FRotator new_pitch{ clamped_pitch, 0.0, 0.0 };
    spring_arm->SetRelativeRotation( new_pitch );
}

void APlayerCharacter::SetCanWalk( bool canWalk ) {
    can_walk = canWalk;
}

bool APlayerCharacter::GetCanWalk() const {
    return can_walk;
}

const FVector &APlayerCharacter::GetLastMovementInput() const {
    return last_movement_input;
}

void APlayerCharacter::SetLastMovementZInput( const float input_value ) {
    last_movement_input.Z = input_value;
}

void APlayerCharacter::UpdateCameraRotation() {
    old_camera_rotation = FRotator( spring_arm->GetRelativeRotation().Pitch, gimbal->GetComponentRotation().Yaw, 0.0 );
    new_camera_rotation = GetActorRotation();

    const FRotator new_rotation = FQuat::Slerp( old_camera_rotation.Quaternion(), new_camera_rotation.Quaternion(), 0.05f ).Rotator();
    const FRotator new_yaw{ 0.0, new_rotation.Yaw, 0.0 };
    const FRotator new_pitch{ new_rotation.Pitch, 0.0, 0.0 };
    gimbal->SetWorldRotation( new_yaw );
    spring_arm->SetRelativeRotation( new_pitch );
}
