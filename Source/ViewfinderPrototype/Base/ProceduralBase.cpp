// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralBase.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "ProceduralPhysics.h"

// Sets default values
AProceduralBase::AProceduralBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));

	ProceduralMeshCom = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMeshCom->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AProceduralBase::BeginPlay()
{
	Super::BeginPlay();


	ProceduralMeshComps.Add(ProceduralMeshCom);
	if (PhotoInformation.ValidPoints.Num() < 0)
	{
		PhotoInformation.ValidPoints.Add(GetRelativeBoundsOrigin(ProceduralMeshCom));
	}
	Camera = Cast<AViewfinderCamera>(GetOwner());
}

void AProceduralBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetupMesh();
}

// Called every frame
void AProceduralBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProceduralBase::SetupMesh()
{
	UStaticMesh* NewStaticMesh = PhotoInformation.StaticMesh;
	UStaticMeshComponent* StaticMeshComp = NewObject<UStaticMeshComponent>(this);
	StaticMeshComp->RegisterComponentWithWorld(GetRootComponent()->GetWorld());
	StaticMeshComp->SetStaticMesh(NewStaticMesh);
	UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(StaticMeshComp, 0, ProceduralMeshCom, true);
	StaticMeshComp->DestroyComponent();


	if (PhotoInformation.Material.Num() != 0)
	{
		for (int32 i = 0; i < PhotoInformation.Material.Num(); i++)
		{
			ProceduralMeshCom->SetMaterial(i, PhotoInformation.Material[i]);
		}
	}
}

void AProceduralBase::DestroyMesh(UProceduralMeshComponent* newProceduralMesh)
{
	UProceduralMeshComponent* proceduralMesh;
	if (IsValid(newProceduralMesh))
	{
		proceduralMesh = newProceduralMesh;
	}
	else
	{
		proceduralMesh = ProceduralMeshCom;
	}

	if (proceduralMesh)
	{
		ProceduralMeshComps.Remove(proceduralMesh);
		proceduralMesh->DestroyComponent();
	}
	TArray<UProceduralMeshComponent*> ActorCom = ProceduralMeshComps;
	if (ActorCom.Num() == 0)
	{
		Destroy();
	}
}

void AProceduralBase::ClearSmallCuts(float MinSize)
{
	TArray<UProceduralMeshComponent*> ActorCom = ProceduralMeshComps;
	for (auto& CompActor : ActorCom)
	{
		FVector Origin;
		FVector Box;
		float sphereRadius;
		UKismetSystemLibrary::GetComponentBounds(Cast<USceneComponent>(CompActor), Origin, Box, sphereRadius);
		if (CompActor->GetOwner() == this)
		{
			if (Box.X < MinSize || Box.Y < MinSize || Box.Z < MinSize)
			{
				DestroyMesh(Cast<UProceduralMeshComponent>(CompActor));
			}
		}
	}
}

void AProceduralBase::DetermineValidPoints(bool NoClearSmall)
{
	if (!NoClearSmall)
	{
		ClearSmallCuts();
	}
	if (!IsActorBeingDestroyed())
	{
		PhotoInformation.ValidPoints.Empty();
		TArray<UProceduralMeshComponent*> ActorCom = ProceduralMeshComps;
		for (auto& CompActor : ActorCom)
		{
			if (CompActor->GetOwner() == this)
			{
				PhotoInformation.ValidPoints.AddUnique(GetRelativeBoundsOrigin(CompActor));
			}
		}
	}
}

bool AProceduralBase::AlmostContains(FVector ValueIn, float Range)
{
	for (auto& var : PhotoInformation.ValidPoints)
	{
		return var.Equals(ValueIn, Range);
	}
	return false;
}

FTransform AProceduralBase::GetProceduralTransform()
{
	if (!IsActorBeingDestroyed())
	{
		AProceduralPhysics* ProcedurPhys = Cast<AProceduralPhysics>(this);
		if (ProcedurPhys)
		{
			if (ProcedurPhys->ProceduralMeshComps[0])
			{
				return ProcedurPhys->ProceduralMeshComps[0]->GetComponentTransform();
			}
		}
		
		return GetActorTransform();
	}
	return FTransform();
}

void AProceduralBase::ClearExtraSlices()
{
	if (PhotoInformation.OldCuts != 0)
	{
		TArray<TPair<FVector, FVector>> PairValue = PhotoInformation.Slices.Array();
		for (int32 i = PairValue.Num(); i < 0; i--)
		{
			if (i >= PhotoInformation.OldCuts)
			{
				PhotoInformation.Slices.Remove(PairValue[i].Key);
			}
		}
	}
	else
	{
		PhotoInformation.Slices.Empty();
	}
}

void AProceduralBase::SliceBasedOnPhoto()
{
	SetupMesh();
	FVector NormalRelative;
	FVector PositionRelative;
	TArray<TPair<FVector, FVector>> PairValue = PhotoInformation.Slices.Array();
	TArray<UProceduralMeshComponent*> CutsToDelete;
	FTransform Transform = GetProceduralTransform();

	if (PhotoInformation.OldCuts != 0)
	{
		for (int32 i = 0; i < PairValue.Num(); i++)
		{
			NormalRelative = PairValue[i].Key;
			PositionRelative = PhotoInformation.Slices[NormalRelative];
			if (i == PhotoInformation.OldCuts)
			{
				break;
			}
			else
			{
				FVector PlanePosition = UKismetMathLibrary::TransformLocation(Transform, PositionRelative);
				FVector PlaneDirection = UKismetMathLibrary::TransformDirection(Transform, NormalRelative);
				SliceMesh(false, PlanePosition, PlaneDirection);
			}
		}
		for (auto& ACFound : ProceduralMeshComps)
		{
			if (ACFound->GetOwner() == this)
			{
				if (AlmostContains(GetRelativeBoundsOrigin(ACFound), 0.1f))
				{
					CutsToDelete.Add(ACFound);
				}
			}
		}
		for (auto& comp : CutsToDelete)
		{
			if (comp)
			{
				DestroyMesh(comp);
			}
		}
	}

	CutsToDelete.Empty();
	for (int32 i = 0; i < PairValue.Num(); i++)
	{
		NormalRelative = PairValue[i].Key;
		PositionRelative = PhotoInformation.Slices[NormalRelative];
		if (i >= PhotoInformation.OldCuts)
		{
			FVector PlanePosition = UKismetMathLibrary::TransformLocation(Transform, PositionRelative);
			FVector PlaneDirection = UKismetMathLibrary::TransformDirection(Transform, NormalRelative);
			SliceMesh(false, PlanePosition, PlaneDirection);
			PhotoInformation.OldCuts++;
		}
	}
	for (auto& ACFound : ProceduralMeshComps)
	{
		if (ACFound->GetOwner() == this)
		{
			FVector Origin;
			FVector Box;
			float sphereRadius;
			UKismetSystemLibrary::GetComponentBounds(ACFound, Origin, Box, sphereRadius);
			if (IsValid(Camera))
			{
				if (!Camera->IsInside(Origin))
				{
					CutsToDelete.Add(ACFound);
				}
			}
		}
	}
	for (auto& comp : CutsToDelete)
	{
		if (comp)
		{
			DestroyMesh(comp);
		}
	}

	DetermineValidPoints(false);
}

void AProceduralBase::SetCamera(AViewfinderCamera* NewCam)
{
	Camera = NewCam;
}

FVector AProceduralBase::GetRelativeBoundsOrigin(UProceduralMeshComponent* SceneCom)
{
	FVector Origin;
	FVector Box;
	float sphereRadius;
	UKismetSystemLibrary::GetComponentBounds(SceneCom, Origin, Box, sphereRadius);
	return UKismetMathLibrary::InverseTransformLocation(GetProceduralTransform(), Origin);
}

TArray<UProceduralMeshComponent*> AProceduralBase::SliceMesh(bool AddCuts, FVector PlanePosition, FVector PlaneNormal)
{
	TArray<UProceduralMeshComponent*> ReturnMesh;
	TArray<UProceduralMeshComponent*> CurrentMesh;
	FTransform Transform = GetProceduralTransform();
	if (AddCuts)
	{
		
		FVector NewKey = UKismetMathLibrary::InverseTransformDirection(Transform, PlaneNormal);
		FVector NewLoc = UKismetMathLibrary::InverseTransformLocation(Transform, PlanePosition);

		PhotoInformation.Slices.Add(NewKey, NewLoc);

		PhotoInformation.OldCuts++;
		
	}

	TArray<UProceduralMeshComponent*> ActorCom = ProceduralMeshComps;
	for (auto& actorComp : ActorCom)
	{
		if (actorComp)
		{
			CurrentMesh.Add(actorComp);
		}
	}

	for (auto& current : CurrentMesh)
	{
		UProceduralMeshComponent* OutProcedural;
		if (current->GetOwner() == this)
		{
			UKismetProceduralMeshLibrary::SliceProceduralMesh(current, PlanePosition, PlaneNormal, true, OutProcedural, EProcMeshSliceCapOption::UseLastSectionForCap, (UMaterialInterface*)nullptr);
			ReturnMesh.Add(OutProcedural);
			ProceduralMeshComps.Add(OutProcedural);
		}
	}
	return ReturnMesh;
}

