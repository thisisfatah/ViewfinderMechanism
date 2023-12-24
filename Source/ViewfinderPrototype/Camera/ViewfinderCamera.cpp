// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewfinderCamera.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralMeshComponent.h"
#include <ViewfinderPrototype/Base/ProceduralBase.h>
#include <ViewfinderPrototype/Base/ProceduralPhysics.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AViewfinderCamera::AViewfinderCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root Component")));
}

// Called when the game starts or when spawned
void AViewfinderCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AViewfinderCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AViewfinderCamera::IsInside(FVector Location)
{
	FVector InverseTransform = UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), Location);
	float AlphaDistance;
	float RelativeY;
	float RelativeZ;
	if (InverseTransform.X >= 0.f)
	{
		AlphaDistance = InverseTransform.X / Range;
		RelativeY = InverseTransform.Y;
		RelativeZ = InverseTransform.Z;
	}
	else
	{
		return false;
	}

	float getLerp = UKismetMathLibrary::Lerp(10.5f, 1050.f, AlphaDistance);
	bool PositiveY = RelativeY < getLerp;
	bool PositiveZ = RelativeZ < getLerp;
	bool NegativeY = RelativeY > (getLerp * -1.f);
	bool NegativeZ = RelativeZ > (getLerp * -1.f);

	return PositiveY && PositiveZ && NegativeY && NegativeZ;
}

TArray<FPhotoInformation> AViewfinderCamera::CapturePhoto()
{
	TArray<FPhotoInformation> CaptureInfo;
	TArray<AProceduralBase*> ActorInFrames = GetItemnIFrames();
	FTransform Transform;

	for (auto& ActorFrame : ActorInFrames)
	{
		if (ActorFrame)
		{
			AProceduralPhysics* ProceduralPhysic = Cast<AProceduralPhysics>(ActorFrame);
			if (ProceduralPhysic)
			{
				UProceduralMeshComponent* MeshCom = ProceduralPhysic->ProceduralMeshComps[0];
				if (MeshCom)
				{
					Transform = MeshCom->GetComponentTransform();
				}
				
			}
			else
			{
				Transform = ActorFrame->GetActorTransform();
			}
			ActorFrame->PhotoInformation.Transform.SetLocation(UKismetMathLibrary::InverseTransformLocation(GetActorTransform(), Transform.GetLocation()));
			ActorFrame->PhotoInformation.Transform.SetRotation(FQuat(UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), Transform.Rotator())));
			ActorFrame->PhotoInformation.Transform.SetScale3D(Transform.GetScale3D());
			FPhotoInformation newPhotoInformation(ActorFrame->PhotoInformation.StaticMesh, ActorFrame->PhotoInformation.Material, ActorFrame->PhotoInformation.Physics, ActorFrame->PhotoInformation.Transform, ActorFrame->PhotoInformation.Slices, ActorFrame->PhotoInformation.ValidPoints, ActorFrame->PhotoInformation.OldCuts);
			CaptureInfo.Add(newPhotoInformation);
			ActorFrame->ClearExtraSlices();
		}
	}

	return 	CaptureInfo;
}
TArray<AProceduralBase*> AViewfinderCamera::GetItemnIFrames()
{
	FVector CutVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), FVector(-0.203f, -0.979, 0.f));

	float SizeInterval = 0.01f;
	float RightAlpha = -1.f;
	float UpAlpha = 1.f;

	int32 Switch = 0;
	int32 Count = 0;

	TArray<AProceduralBase*> AlreadyCutActors;
	TArray<AProceduralBase*> AlreadyCutActorsModified;

	while (Count < UKismetMathLibrary::Round(2 / SizeInterval) * 4)
	{
		FVector StartTrace = GetActorLocation() + GetActorRightVector() * (RightAlpha * 10.45f) + GetActorUpVector() * (UpAlpha * 10.45f);
		FVector EndTrace = (GetActorLocation() + GetActorForwardVector() * Range) + GetActorRightVector() * (RightAlpha * 1045.f) + GetActorUpVector() * (UpAlpha * 1045.f);
		TArray<AActor*> ActorToIgnore;
		TArray<FHitResult> OutHits;
		bool OnHits = UKismetSystemLibrary::LineTraceMulti(
			this,
			StartTrace,
			EndTrace,
			TraceTypeQuery3,
			false,
			ActorToIgnore,
			EDrawDebugTrace::None,
			OutHits,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			5.f
		);
		for (auto& Hit : OutHits)
		{
			AProceduralBase* Base = Cast<AProceduralBase>(Hit.GetActor());
			AlreadyCutActors.AddUnique(Base);
			if (!AlreadyCutActorsModified.Contains(Base))
			{
				AlreadyCutActorsModified.Add(Base);
				UProceduralMeshComponent* MeshCom = Cast<UProceduralMeshComponent>(Hit.GetComponent());
				if (MeshCom)
				{
					Base->PhotoInformation.Slices.Add(UKismetMathLibrary::InverseTransformDirection(Base->GetProceduralTransform(), CutVector), UKismetMathLibrary::InverseTransformLocation(Base->GetProceduralTransform(), Hit.ImpactPoint));
				}
			}
		}
		Count++;
		switch (Switch)
		{
		case 0:
			UpAlpha = UpAlpha - SizeInterval;
			if (UKismetMathLibrary::NearlyEqual_FloatFloat(UpAlpha, -1.f, 0.001f))
			{
				Switch++;
				CutVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), FVector(-0.203, 0.f, -0.979));
				AlreadyCutActorsModified.Empty();
			}
			break;
		case 1:
			RightAlpha = RightAlpha + SizeInterval;
			if (UKismetMathLibrary::NearlyEqual_FloatFloat(RightAlpha, 1.f, 0.001f))
			{
				Switch++;
				CutVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), FVector(-0.203, 0.979, 0));
				AlreadyCutActorsModified.Empty();
			}
			break;
		case 2:
			UpAlpha = UpAlpha + SizeInterval;
			if (UKismetMathLibrary::NearlyEqual_FloatFloat(UpAlpha, 1.f, 0.001f))
			{
				Switch++;
				CutVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), FVector(-0.203, 0.f, 0.979));
				AlreadyCutActorsModified.Empty();
			}
			break;
		case 3:
			RightAlpha = RightAlpha - SizeInterval;
			break;
		}
	}

	AlreadyCutActors.Append(GetInsideActors(AlreadyCutActors));


	return AlreadyCutActors;
}

void AViewfinderCamera::CutItemInsideFrame()
{
	FVector CutVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), FVector(-0.203f, -0.979, 0.f));

	float SizeInterval = 0.01f;
	float RightAlpha = -1.f;
	float UpAlpha = 1.f;

	int32 Count = 0;
	int32 Switch = 0;

	TArray<AProceduralBase*> AlreadyCutActors;
	TArray<AProceduralBase*> AlreadyCutActorsModified;
	TArray<UProceduralMeshComponent*> MeshesToDelete;

	while (Count < UKismetMathLibrary::Round(2 / SizeInterval) * 4)
	{
		FVector StartTrace = GetActorLocation() + GetActorRightVector() * (RightAlpha * 10.45f) + GetActorUpVector() * (UpAlpha * 10.45f);
		FVector EndTrace = (GetActorLocation() + GetActorForwardVector() * Range) + GetActorRightVector() * (RightAlpha * 1045.f) + GetActorUpVector() * (UpAlpha * 1045.f);
		TArray<AActor*> ActorToIgnore;
		TArray<FHitResult> OutHits;
		bool OnHits = UKismetSystemLibrary::LineTraceMulti(
			this,
			StartTrace,
			EndTrace,
			TraceTypeQuery3,
			false,
			ActorToIgnore,
			EDrawDebugTrace::None,
			OutHits,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			1500.f
		);
		for (auto& Hit : OutHits)
		{
			AProceduralBase* Base = Cast<AProceduralBase>(Hit.GetActor());
			if (Base)
			{
				AlreadyCutActors.AddUnique(Base);
				if (!AlreadyCutActorsModified.Contains(Base))
				{
					AlreadyCutActorsModified.Add(Base);
					UProceduralMeshComponent* MeshCom = Cast<UProceduralMeshComponent>(Hit.GetComponent());
					if (MeshCom)
					{
						MeshesToDelete.Append(Base->SliceMesh(true, Hit.ImpactPoint, CutVector));
					}
				}
			}
		}
		Count++;
		switch (Switch)
		{
		case 0:
			UpAlpha = UpAlpha - SizeInterval;
			if (UKismetMathLibrary::NearlyEqual_FloatFloat(UpAlpha, -1.f, 0.001f))
			{
				Switch++;
				CutVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), FVector(-0.203, 0.f, -0.979));
				AlreadyCutActorsModified.Empty();
			}
			break;
		case 1:
			RightAlpha = RightAlpha + SizeInterval;
			if (UKismetMathLibrary::NearlyEqual_FloatFloat(RightAlpha, 1.f, 0.001f))
			{
				Switch++;
				CutVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), FVector(-0.203, 0.979, 0));
				AlreadyCutActorsModified.Empty();
			}
			break;
		case 2:
			UpAlpha = UpAlpha + SizeInterval;
			if (UKismetMathLibrary::NearlyEqual_FloatFloat(UpAlpha, 1.f, 0.001f))
			{
				Switch++;
				CutVector = UKismetMathLibrary::TransformDirection(GetActorTransform(), FVector(-0.203, 0.f, 0.979));
				AlreadyCutActorsModified.Empty();
			}
			break;
		case 3:
			RightAlpha = RightAlpha - SizeInterval;
			break;
		}
	}

	DestroyActorsInside(AlreadyCutActors, MeshesToDelete);
}

TArray<AProceduralBase*> AViewfinderCamera::GetInsideActors(TArray<AProceduralBase*> Ignore)
{
	TArray<AActor*> BaseActors;
	TArray<AProceduralBase*> ActorInsides;
	FVector Location;
	UGameplayStatics::GetAllActorsOfClass(this, AProceduralBase::StaticClass(), BaseActors);
	for (auto& Base : BaseActors)
	{
		AProceduralPhysics* basePhys = Cast<AProceduralPhysics>(Base);
		if (basePhys)
		{
			UProceduralMeshComponent* CompPhys = basePhys->ProceduralMeshComps[0];
			if (CompPhys)
			{
				Location = CompPhys->GetComponentLocation();
			}
		}
		else
		{
			Location = Base->GetActorLocation();
		}
		if (IsInside(Location) && !Ignore.Contains(Base))
		{
			ActorInsides.Add(Cast<AProceduralBase>(Base));
		}
	}
	return ActorInsides;
}

void AViewfinderCamera::DestroyActorsInside(TArray<AProceduralBase*> Ignore, TArray<UProceduralMeshComponent*> MeshesToCut)
{
	TArray<AActor*> BaseActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProceduralBase::StaticClass(), BaseActors);

	TArray<AProceduralBase*> ActorCuts;
	FVector Location;

	for (auto& Base : BaseActors)
	{
		AProceduralBase* baseProced = Cast<AProceduralBase>(Base);
		if (baseProced)
		{
			AProceduralPhysics* basePhys = Cast<AProceduralPhysics>(baseProced);
			if (basePhys)
			{
				UProceduralMeshComponent* CompPhys = basePhys->ProceduralMeshComps[0];
				if (CompPhys)
				{
					Location = CompPhys->GetComponentLocation();
				}
			}
			else
			{
				Location = baseProced->GetActorLocation();
			}
			if (IsInside(Location) && !Ignore.Contains(baseProced))
			{
				baseProced->DestroyMesh(nullptr);
			}
		}
	}
	for (auto& Meshes : MeshesToCut)
	{
		FVector Origin;
		FVector BoxExtent;
		float SphereRadius;
		UKismetSystemLibrary::GetComponentBounds(Meshes, Origin, BoxExtent, SphereRadius);
		if (IsInside(Origin))
		{
			AProceduralBase* BaseProcedural = Cast<AProceduralBase>(Meshes->GetOwner());
			if (BaseProcedural)
			{
				BaseProcedural->DestroyMesh(Meshes);
			}
		}
		else
		{
			AProceduralBase* BaseProcedural = Cast<AProceduralBase>(Meshes->GetOwner());
			if (BaseProcedural)
			{
				ActorCuts.AddUnique(BaseProcedural);
			}
		}
	}

	for (auto& ActorCut : ActorCuts)
	{
		ActorCut->DetermineValidPoints(true);
	}
}

void AViewfinderCamera::PlaceNewItems(TArray<FPhotoInformation> PhotoInformation)
{
	for (auto& PhotoInfo : PhotoInformation)
	{
		if (PhotoInfo.Physics)
		{
			FTransform itemTransform = PhotoInfo.Transform;
			FTransform Transform = UKismetMathLibrary::MakeTransform(UKismetMathLibrary::TransformLocation(GetActorTransform(), itemTransform.GetLocation()), UKismetMathLibrary::TransformRotation(GetActorTransform(), itemTransform.Rotator()));
			FActorSpawnParameters ParamSpawn = FActorSpawnParameters();
			ParamSpawn.Owner = this;
			AProceduralPhysics* Item = GetWorld()->SpawnActor<AProceduralPhysics>(ItemPhysicSpawn, Transform, ParamSpawn);
			if (Item)
			{
				Item->PhotoInformation = PhotoInfo;
				Item->SliceBasedOnPhoto();
			}
		}
		else
		{
			FTransform itemTransform = PhotoInfo.Transform;
			FTransform Transform = UKismetMathLibrary::MakeTransform(UKismetMathLibrary::TransformLocation(GetActorTransform(), itemTransform.GetLocation()), UKismetMathLibrary::TransformRotation(GetActorTransform(), itemTransform.Rotator()));
			FActorSpawnParameters ParamSpawn = FActorSpawnParameters();
			ParamSpawn.Owner = this;
			AProceduralBase* Item = GetWorld()->SpawnActor<AProceduralBase>(ItemSpawn, Transform, ParamSpawn);
			if (Item)
			{
				Item->PhotoInformation = PhotoInfo;
				Item->SliceBasedOnPhoto();
			}
		}
	}
}

