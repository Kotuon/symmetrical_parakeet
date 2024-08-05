// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Action.h"
#include "GravityRush.generated.h"

class UInputAction;

UCLASS()
class SYMMETRICAL_PARAKEET_API UGravityRush : public UAction {
    GENERATED_BODY()
protected: // Functions
    virtual void BeginPlay() override;

public: // Functions
    UGravityRush();
    virtual void Start( const FInputActionValue &value );
    virtual void End();

    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) override;

private: // Functions
public:  // Variables
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Input" )
    UInputAction *should_move_action;

private: // Variables
    FVector direction_to_move;

    bool should_move = false;
    bool gravity_mode = false;
};
