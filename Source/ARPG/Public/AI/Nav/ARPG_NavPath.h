﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "ARPG_NavPath.generated.h"

USTRUCT(BlueprintType)
struct FARPG_NavPathPoint
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "路径")
	FVector Location;

	UPROPERTY(EditAnywhere, Category = "路径")
	FRotator Rotation;

 	UPROPERTY(EditAnywhere, Category = "路径", Instanced)
 	class UARPG_CharacterBehaviorConfigBase* Behavior = nullptr;
};

UCLASS()
class ARPG_API UARPG_NavPathVisualHelper : public USplineComponent
{
	GENERATED_BODY()
public:

};

UCLASS(meta = (DisplayName = "路径"))
class ARPG_API AARPG_NavPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AARPG_NavPath();

	UPROPERTY(EditAnywhere, Category = "路径")
	uint8 bIsClosedLoop : 1;

	UPROPERTY(EditAnywhere, Category = "路径")
	TArray<FARPG_NavPathPoint> NavPathPoints;

#if WITH_EDITORONLY_DATA
	class UARPG_NavPathVisualHelper* VisualControl;

	void OnConstruction(const FTransform& Transform) override;
#endif
};


