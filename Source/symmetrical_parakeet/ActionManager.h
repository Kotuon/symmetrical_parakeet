#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionTypes.h"
#include "ActionManager.generated.h"

class UAction;

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SYMMETRICAL_PARAKEET_API UActionManager : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UActionManager();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) override;

    UFUNCTION( BlueprintCallable )
    bool StartAction( EAction type );
    UFUNCTION( BlueprintCallable )
    void EndAction( EAction type );

    UAction *GetAction( EAction type );

    EAction GetCurrentActionType() const;
    EAction GetLastActionType() const;
    EAction GetNextActionType() const;

    const TArray< UAction * > &GetActionArray() const;

    bool IsBusy();

private:
    TMap< EAction, UAction * > action_map;
    TArray< UAction * > action_array;
    AActor *parent;
    EAction current_action = EAction::A_None;
    EAction last_action = EAction::A_None;
    EAction next_action = EAction::A_None;
};