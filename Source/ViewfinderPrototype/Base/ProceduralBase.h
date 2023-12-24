// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "../Struct/SPhotoInformation.h"
#include "../Camera/ViewfinderCamera.h"
#include "ProceduralBase.generated.h"

UCLASS()
class VIEWFINDERPROTOTYPE_API AProceduralBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform);

	
public:	

	UPROPERTY(EditDefaultsOnly)
		UProceduralMeshComponent* ProceduralMeshCom;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, Category = "ProceduralBase|Settings")
		AViewfinderCamera* Camera;

	TSubclassOf<UProceduralMeshComponent> ProceduralMeshSubClass;

public:
	UPROPERTY(EditAnywhere, Category = "ProceduralBase|Settings")
		FPhotoInformation PhotoInformation;

	TArray<UProceduralMeshComponent*> ProceduralMeshComps;

public:
	void ClearExtraSlices();

	void DestroyMesh(UProceduralMeshComponent* proceduralMesh);

	void DetermineValidPoints(bool NoClearSmall);

	void SliceBasedOnPhoto();

	void SetCamera(AViewfinderCamera* NewCam);

	void SetupMesh();

	TArray<UProceduralMeshComponent*> SliceMesh(bool AddCuts, FVector PlanePosition, FVector PlaneNormal);

	FTransform GetProceduralTransform();

protected:

	void ClearSmallCuts(float MinSize = 5.f);

	bool AlmostContains(FVector ValueIn, float Range = 0.5f);


	FVector GetRelativeBoundsOrigin(UProceduralMeshComponent* SceneCom);
};
