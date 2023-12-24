// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralPhysics.h"

// Sets default values
AProceduralPhysics::AProceduralPhysics()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshCom->bUseComplexAsSimpleCollision = false;
}

// Called when the game starts or when spawned
void AProceduralPhysics::BeginPlay()
{
	Super::BeginPlay();
	
	ProceduralMeshCom->SetMobility(EComponentMobility::Movable);
	ProceduralMeshCom->SetSimulatePhysics(true);
}

// Called every frame
void AProceduralPhysics::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

