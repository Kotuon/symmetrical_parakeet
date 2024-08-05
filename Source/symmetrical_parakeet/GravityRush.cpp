// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityRush.h"

UGravityRush::UGravityRush() {
    type = EAction::A_GravityRush;
    PrimaryComponentTick.bCanEverTick = true;
    SetComponentTickEnabled( true );
}

void UGravityRush::BeginPlay() {
    Super::BeginPlay();
    //...
}

void UGravityRush::Start( const FInputActionValue &value ) {
}

void UGravityRush::End() {
    Super::End();
    //...
}

void UGravityRush::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) {
}