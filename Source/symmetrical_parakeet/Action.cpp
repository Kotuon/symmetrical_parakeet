#include "Action.h"
#include "ActionManager.h"   // UActionManager class
#include "PlayerCharacter.h" // APlayerCharacter class

// Sets default values for this component's properties
UAction::UAction() {
    PrimaryComponentTick.bCanEverTick = false;
    // ...
}

// Called when the game starts
void UAction::BeginPlay() {
    Super::BeginPlay();
    // ...

    parent = Cast< APlayerCharacter >( GetOwner() );
}

void UAction::Start( const FInputActionValue &value ) {
}
void UAction::Update() {
}
void UAction::End() {
    manager->EndAction( type );
}

void UAction::StartNotifyWindow() {
}
void UAction::EndNotifyWindow() {
}

// Called every frame
void UAction::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    // ...
}

void UAction::Print() const {
    FString name;

    if ( type == EAction::A_None )
        name += "None";
    else if ( type == EAction::A_Jump )
        name += "Jump";

    GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Cyan, FString( "Found: " + name + " : " + FString::FromInt( uint8( type ) ) ) );
}
