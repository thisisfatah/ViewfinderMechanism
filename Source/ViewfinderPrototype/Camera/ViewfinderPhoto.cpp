// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewfinderPhoto.h"

// Sets default values
AViewfinderPhoto::AViewfinderPhoto()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AViewfinderPhoto::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AViewfinderPhoto::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

