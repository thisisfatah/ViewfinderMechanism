// Fill out your copyright notice in the Description page of Project Settings.


#include "AC_Viewfinder.h"
#include "Kismet/GameplayStatics.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include "Camera/CameraComponent.h"
#include "../ViewfinderPrototypeCharacter.h"
#include <ViewfinderPrototype/Camera/ViewfinderCamera.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values for this component's properties
UAC_Viewfinder::UAC_Viewfinder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_Viewfinder::BeginPlay()
{
	Super::BeginPlay();

	// ...
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UEnhancedInputLocalPlayerSubsystem* SubSytem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	SubSytem->AddMappingContext(IMC_Viewfinder, 0);

	UMGCamera = Cast<UUMGCamera>(CreateWidget(GetWorld(), UMGCameraClass));
	SceneCapture2D = Cast<USceneCaptureComponent2D>(GetOwner()->AddComponentByClass(SceneCapture2DClass, true, FTransform(FRotator(0.f, 0.f, 0.f), FVector(10.f, 0.f, 0.f), FVector(1.f, 1.f, 1.f)), false));
	SceneCapture2D->AttachToComponent(Cast<AViewfinderPrototypeCharacter>(GetOwner())->GetFirstPersonCameraComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SceneCapture2D->TextureTarget = PhotoTexture;
	SceneCapture2D->bCaptureEveryFrame = false;
	SceneCapture2D->bCaptureOnMovement = false;
	SceneCapture2D->bAlwaysPersistRenderingState = true;
	SceneCapture2D->FOVAngle = 23.5f;
}

// Called every frame
void UAC_Viewfinder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAC_Viewfinder::SetupInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(IA_Zoom, ETriggerEvent::Started, this, &UAC_Viewfinder::TriggerStartZoom);
	EnhancedInputComponent->BindAction(IA_Zoom, ETriggerEvent::Completed, this, &UAC_Viewfinder::TriggerCompleteZoom);

	EnhancedInputComponent->BindAction(IA_Capture, ETriggerEvent::Started, this, &UAC_Viewfinder::TriggerStartCapture);
	//EnhancedInputComponent->BindAction(IA_Capture, ETriggerEvent::Completed, this, &UAC_Viewfinder::TriggerCompleteCapture);
}

void UAC_Viewfinder::TriggerStartZoom()
{
	if (HasPhoto) {
		FocusOnPhoto();
	}
	else {
		UMGCamera->AddToViewport();
		UMGCamera->SetVisibility(ESlateVisibility::Visible);
	}
}

void UAC_Viewfinder::TriggerCompleteZoom()
{
	if (HasPhoto) {
		LoseFocusOnPhoto();
	}
	else {
		UMGCamera->RemoveFromParent();
	}
}

void UAC_Viewfinder::TriggerStartCapture()
{
	if (HasPhoto) {
		PlacePhoto();
	}
	else {
		if (UMGCamera->IsVisible()) {
			CapturePhoto();	
			UMGCamera->RemoveFromParent();
		}
	}
}

void UAC_Viewfinder::TriggerCompleteCapture()
{
}

void UAC_Viewfinder::FocusOnPhoto()
{
	Photo->SetActorRelativeLocation(FVector(50.f, 0.f, 0.f), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	Photo->SetActorRelativeRotation(FRotator(90.f, 0.f, 0.f), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	IsFocused = true;
}

void UAC_Viewfinder::CapturePhoto()
{
	Photo = Cast<AViewfinderPhoto>(GetWorld()->SpawnActor<AViewfinderPhoto>(PhotoClass, FTransform(FRotator(80.f, 0.f, 0.f), FVector::ZeroVector, FVector::OneVector)));
	Photo->AttachToComponent(SceneCapture2D, FAttachmentTransformRules::KeepRelativeTransform);
	Photo->SetActorRelativeLocation(FVector(45.f, -30.f, -15.f), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	SceneCapture2D->CaptureScene();
	HasPhoto = true;
	AViewfinderCamera* Camera = GetWorld()->SpawnActor<AViewfinderCamera>(CameraClass, GetCameraTransform());
	if (Camera)
	{
		Photo->PhotoInformation = Camera->CapturePhoto();
		Camera->Destroy();
	}
}

void UAC_Viewfinder::PlacePhoto()
{
	if (IsFocused) 
	{
		AViewfinderCamera* Camera = GetWorld()->SpawnActor<AViewfinderCamera>(CameraClass, GetCameraTransform());
		if (Camera)
		{
			Camera->CutItemInsideFrame();
			Camera->PlaceNewItems(Photo->PhotoInformation);
			Camera->Destroy();
		}
		HasPhoto = false;
		Photo->Destroy();
	}
}

FTransform UAC_Viewfinder::GetCameraTransform()
{
	FVector Location = SceneCapture2D->GetComponentLocation() + SceneCapture2D->GetForwardVector() * 50.f;
	FRotator Rotation = SceneCapture2D->GetComponentRotation();
	
	return UKismetMathLibrary::MakeTransform(Location, Rotation);
}

void UAC_Viewfinder::LoseFocusOnPhoto()
{
	Photo->SetActorRelativeLocation(FVector(45.f, -30.f, -15.f), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	Photo->SetActorRelativeRotation(FRotator(80.f, 0.f, 0.f), false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	IsFocused = false;
}

