// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBase.h"
#include "ProceduralPhysics.generated.h"

UCLASS()
class VIEWFINDERPROTOTYPE_API AProceduralPhysics : public AProceduralBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralPhysics();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
