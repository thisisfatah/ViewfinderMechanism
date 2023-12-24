// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "../Struct/SPhotoInformation.h"
#include "ProceduralMeshComponent.h"
#include "ViewfinderPhoto.generated.h"

UCLASS()
class VIEWFINDERPROTOTYPE_API AViewfinderPhoto : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AViewfinderPhoto();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	USceneComponent* Root;

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* StaticMesh;

public:
	TArray<FPhotoInformation> PhotoInformation;
};
