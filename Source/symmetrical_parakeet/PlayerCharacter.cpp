
#include "PlayerCharacter.h"
#include "SmartSpringArm.h"                           // USmartSpringArm class
#include "Camera/CameraComponent.h"                   // UCameraComponent class
#include "EnhancedInputSubsystems.h"                  // UEnhancedInputLocalPlayerSubsystem class
#include "EnhancedInputComponent.h"                   // UEnhancedInputComponent class
#include "Kismet/KismetMathLibrary.h"                 // GetForwardVector(), GetRightVector()
#include "GameFramework/CharacterMovementComponent.h" // UCharacterMovementComponent class
#include "Action.h"                                   // UAction class
#include "ActionManager.h"                            // UActionManager class
#include "GameFramework/MovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter( const FObjectInitializer &ObjectInitializer ) : ACharacter( ObjectInitializer ) {
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    action_manager = Cast< UActionManager >( CreateDefaultSubobject< UActionManager >( FName( "ActionManager" ) ) );

    gimbal = Cast< USceneComponent >( CreateDefaultSubobject< USceneComponent >( FName( "Gimbal" ) ) );
    gimbal->SetupAttachment( GetRootComponent() );
    spring_arm = Cast< USmartSpringArm >( CreateDefaultSubobject< USmartSpringArm >( FName( "SpringArm" ) ) );
    spring_arm->SetupAttachment( gimbal );
    camera = CreateDefaultSubobject< UCameraComponent >( FName( "Camera" ) );
    camera->SetupAttachment( spring_arm );
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



    // FHitResult hit_result;
    // const FQuat camera_rotation = FNinjaMath::MakeFromZQuat(GetActorUpVector(), gimbal->GetComponentQuat() );
    // GetNinjaCharacterMovement()->SafeMoveUpdatedComponent(FVector::ZeroVector, camera_rotation, false, hit_result, ETeleportType::TeleportPhysics);




    // FRotator gimbal_rotation_yaw = FRotator{ 0.0, gimbal->GetComponentRotation().Yaw, 0.0 };
    // FVector world_forward = UKismetMathLibrary::GetForwardVector( gimbal_rotation_yaw );
    // FVector world_right = UKismetMathLibrary::GetRightVector( gimbal_rotation_yaw );

    // FVector up = GetNinjaCharacterMovement()->GetComponentAxisZ();

    // FVector a1 = FVector::VectorPlaneProject( world_forward, up );
    // FVector a2 = FVector::VectorPlaneProject( world_right, up );

    // DrawDebugLine( GetWorld(), GetActorLocation(), GetActorLocation() + ( a1 * 100.f ), FColor::Green, false, 0.f, ( uint8 )0U, 5.f );
    // DrawDebugLine( GetWorld(), GetActorLocation(), GetActorLocation() + ( a2 * 100.f ), FColor::Red, false, 0.f, ( uint8 )0U, 2.f );
    // DrawDebugLine( GetWorld(), GetActorLocation(), GetActorLocation() + ( up * 100.f ), FColor::Yellow, false, 0.f, ( uint8 )0U, 2.f );
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent( UInputComponent *PlayerInputComponent ) {
    // Get the player controller
    APlayerController *pc = Cast< APlayerController >( GetController() );

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
    FVector2D input_value = value.Get< FVector2D >();
    last_movement_input = input_value;

    if ( !can_walk )
        return;

    FRotator gimbal_rotation_yaw = FRotator{ 0.0, gimbal->GetComponentRotation().Yaw, 0.0 };
    FVector world_forward = UKismetMathLibrary::GetForwardVector( gimbal_rotation_yaw );
    FVector world_right = UKismetMathLibrary::GetRightVector( gimbal_rotation_yaw );

    // FVector up = GetActorUpVector();//GetNinjaCharacterMovement()->GetComponentAxisZ();

    // AddMovementInput( FVector::VectorPlaneProject( world_forward, up ), input_value.Y );
    // AddMovementInput( FVector::VectorPlaneProject( world_right, up ), input_value.X );

    AddMovementInput( world_forward, input_value.Y );
    AddMovementInput( world_right, input_value.X );
}

void APlayerCharacter::Look( const FInputActionValue &value ) {
    FVector2D input_value = value.Get< FVector2D >();

    // FRotator new_yaw = GetActorRightVector().Rotation() * input_value.X * sensitivity * 0.01f;
    FRotator new_yaw{ 0.0, input_value.X * sensitivity, 0.0 };
    gimbal->AddWorldRotation( new_yaw );

    float input_pitch = spring_arm->GetRelativeRotation().Pitch + ( input_value.Y * sensitivity );
    float clamped_pitch = FMath::Clamp( input_pitch, -89, 90 );

    FRotator new_pitch{ clamped_pitch, 0.0, 0.0 };
    spring_arm->SetRelativeRotation( new_pitch );
}

void APlayerCharacter::StartPlayerRotation() {
    old_player_rotation = GetActorRotation();
    new_player_rotation = gimbal->GetRelativeRotation();
    timeSinceRotateStarted = 0.f;
    GetWorldTimerManager().SetTimer( player_rotation_time_handle, this, &APlayerCharacter::UpdatePlayerRotation, 0.01f, true, 0.f );
}

void APlayerCharacter::UpdatePlayerRotation() {
    timeSinceRotateStarted += 0.01f;
    float t = timeSinceRotateStarted / 0.15f;

    if ( t > 1.f ) {
        GetWorldTimerManager().ClearTimer( player_rotation_time_handle );
        return;
    }

    FRotator new_rotation = FQuat::Slerp( old_player_rotation.Quaternion(), new_player_rotation.Quaternion(), t ).Rotator();
    SetActorRotation( new_rotation );
}

void APlayerCharacter::SetCanWalk( bool canWalk ) {
    can_walk = canWalk;
}

const FVector2D &APlayerCharacter::GetLastMovementInput() const {
    return last_movement_input;
}

void APlayerCharacter::UpdateCameraRotation() {
    old_camera_rotation = FRotator( spring_arm->GetRelativeRotation().Pitch, gimbal->GetComponentRotation().Yaw, 0.0 );
    new_camera_rotation = GetActorRotation();

    FRotator new_rotation = FQuat::Slerp( old_camera_rotation.Quaternion(), new_camera_rotation.Quaternion(), 0.05f ).Rotator();
    FRotator new_yaw{ 0.0, new_rotation.Yaw, 0.0 };
    FRotator new_pitch{ new_rotation.Pitch, 0.0, 0.0 };
    gimbal->SetWorldRotation( new_yaw );
    spring_arm->SetRelativeRotation( new_pitch );
}
