// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "SmartSpringArm.generated.h"

class ACharacter;

UCLASS( ClassGroup = "Camera", meta = ( BlueprintSpawnableComponent ) )
class SYMMETRICAL_PARAKEET_API USmartSpringArm : public USpringArmComponent {
    GENERATED_BODY()
protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    virtual void TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction ) override;

public:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Arm Lerp" )
    TArray< float > lerpPoints{ 50, 50, 50, 200 };

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Arm Lerp" )
    float lerpTime = 1.f;

private:
    ACharacter *parent;
    USceneComponent *gimbal;
    float lastT;
    float currTime;
};