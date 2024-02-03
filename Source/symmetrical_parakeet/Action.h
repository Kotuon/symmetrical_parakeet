
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionTypes.h"
#include "InputActionValue.h"
#include "Action.generated.h"

class UActionManager;
class UInputAction;
class APlayerCharacter;

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class SYMMETRICAL_PARAKEET_API UAction : public UActorComponent {
    GENERATED_BODY()

protected: // Functions
    // Called when the game starts
    virtual void BeginPlay() override;

public: // Functions
    // Sets default values for this component's properties
    UAction();

    virtual void Start( const FInputActionValue &value );
    virtual void Update();

    UFUNCTION( BlueprintCallable )
    virtual void End();

    virtual void StartNotifyWindow();
    virtual void EndNotifyWindow();

    void Print() const;

    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) override;

public: // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Input" )
    UInputAction *input_action;

    UActionManager *manager;
    APlayerCharacter *parent;

    UPROPERTY( VisibleAnywhere )
    EAction type;

private: // Variables
    bool is_enabled = true;
};