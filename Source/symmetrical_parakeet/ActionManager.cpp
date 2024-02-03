#include "ActionManager.h"
#include "Action.h"

UActionManager::UActionManager() {
    PrimaryComponentTick.bCanEverTick = false;
}

void UActionManager::BeginPlay() {
    Super::BeginPlay();

    parent = Cast< AActor >( GetOwner() );

    GetOwner()->GetComponents< UAction >( action_array );
    for ( UAction *action : action_array ) {
        action->manager = this;
        action_map.Add( action->type, action );
        // action->Print();
    }
}

void UActionManager::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

bool UActionManager::StartAction( EAction type ) {
    if ( type <= current_action ) return false;

    next_action = type;
    last_action = current_action;

    UAction **action = action_map.Find( current_action );
    if ( action ) ( *action )->End();

    current_action = next_action;
    next_action = EAction::A_None;

    return true;
}

void UActionManager::EndAction( EAction type ) {
    if ( type == current_action )
        current_action = EAction::A_None;
}

UAction *UActionManager::GetAction( EAction type ) {
    if ( !action_map.Contains( type ) ) return nullptr;

    return action_map[type];
}

EAction UActionManager::GetCurrentActionType() const {
    return current_action;
}

EAction UActionManager::GetLastActionType() const {
    return last_action;
}

EAction UActionManager::GetNextActionType() const {
    return next_action;
}

const TArray< UAction * > &UActionManager::GetActionArray() const {
    return action_array;
}

bool UActionManager::IsBusy() {
    return ( current_action != EAction::A_None );
}