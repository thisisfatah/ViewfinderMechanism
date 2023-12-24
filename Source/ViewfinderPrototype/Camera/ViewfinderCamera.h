// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <ViewfinderPrototype/Struct/SPhotoInformation.h>
#include "ViewfinderCamera.generated.h"

class AProceduralBase;
class AProceduralPhysics;
class UProceduralMeshComponent;

UCLASS()
class VIEWFINDERPROTOTYPE_API AViewfinderCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AViewfinderCamera();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsInside(FVector Location);

	TArray<FPhotoInformation> CapturePhoto();
	TArray<AProceduralBase*> GetItemnIFrames();
	void CutItemInsideFrame();
	TArray<AProceduralBase*> GetInsideActors(TArray<AProceduralBase*> AlreadyCutActors);
	void DestroyActorsInside(TArray<AProceduralBase*> Ignore, TArray<UProceduralMeshComponent*> MeshesToCut);
	void PlaceNewItems(TArray<FPhotoInformation> PhotoInformation);
protected:
	UPROPERTY(EditAnywhere, Category = Settings)
		float Range = 5000.f;
	UPROPERTY(EditAnywhere, Category = Settings)
		TSubclassOf<AProceduralBase> ItemSpawn;
	UPROPERTY(EditAnywhere, Category = Settings)
		TSubclassOf<AProceduralPhysics> ItemPhysicSpawn;
};
