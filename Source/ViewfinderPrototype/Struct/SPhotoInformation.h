// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "SPhotoInformation.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPhotoInformation
{

public:

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		UStaticMesh* StaticMesh;
	UPROPERTY(EditAnywhere)
		TArray<UMaterialInterface*> Material;
	UPROPERTY(EditAnywhere)
		bool Physics;
	UPROPERTY(EditAnywhere)
		FTransform Transform;
	UPROPERTY(EditAnywhere, meta = (ToolTip = "KEYS = Normal - VALUES = Position"))
		TMap<FVector, FVector> Slices;
	UPROPERTY(EditAnywhere)
		TArray<FVector> ValidPoints;
	UPROPERTY(EditAnywhere)
		int32 OldCuts;

	FPhotoInformation()
	{

	}

	explicit FORCEINLINE FPhotoInformation(UStaticMesh* newStaticMesh, TArray<UMaterialInterface*> newMat, bool physics, FTransform newTransform, TMap<FVector, FVector> newSlices, TArray<FVector> newValidPoint, int32 oldCuts);
};

FORCEINLINE FPhotoInformation::FPhotoInformation(UStaticMesh* newStaticMesh, TArray<UMaterialInterface*> newMat, bool physics, FTransform newTransform, TMap<FVector, FVector> newSlices, TArray<FVector> newValidPoint, int32 oldCuts)
	: StaticMesh(newStaticMesh)
	, Material(newMat)
	, Physics(physics)
	, Transform(newTransform)
	, Slices(newSlices)
	, ValidPoints(newValidPoint)
	, OldCuts(oldCuts)
{}