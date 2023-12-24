// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "../Widget/UMGCamera.h"
#include "../Camera/ViewfinderPhoto.h"
#include "ProceduralMeshComponent.h"
#include "AC_Viewfinder.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VIEWFINDERPROTOTYPE_API UAC_Viewfinder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_Viewfinder();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
		UInputMappingContext* IMC_Viewfinder;

	UPROPERTY(EditAnywhere, Category = "Input")
		UInputAction* IA_Zoom;

	UPROPERTY(EditAnywhere, Category = "Input")
		UInputAction* IA_Capture;

	UPROPERTY(EditAnywhere, Category = "Input")
		UUMGCamera* UMGCamera;

	UPROPERTY(EditAnywhere, Category = "Input")
		TSubclassOf<UUMGCamera> UMGCameraClass;

	UPROPERTY(EditAnywhere, Category = "Component")
		TSubclassOf<USceneCaptureComponent2D> SceneCapture2DClass;

	UPROPERTY(EditAnywhere, Category = "Component")
		USceneCaptureComponent2D* SceneCapture2D;

	UPROPERTY(EditAnywhere, Category = "Component")
		TObjectPtr<class UTextureRenderTarget2D> PhotoTexture;

	UPROPERTY(EditAnywhere, Category = "Component")
		TSubclassOf<AViewfinderPhoto> PhotoClass;

	UPROPERTY(EditAnywhere, Category = "Component")
		TSubclassOf<class AViewfinderCamera> CameraClass;

	bool HasPhoto;
	bool IsFocused;

	AViewfinderPhoto* Photo;

	UProceduralMeshComponent* Procedural;

protected:

	UFUNCTION()
		void TriggerStartZoom();

	UFUNCTION()
		void TriggerCompleteZoom();

	UFUNCTION()
		void TriggerStartCapture();

	UFUNCTION()
		void TriggerCompleteCapture();
	
	void FocusOnPhoto();
	void LoseFocusOnPhoto();

	void CapturePhoto();
	void PlacePhoto();

	FTransform GetCameraTransform();

public:
	void SetupInputComponent(UEnhancedInputComponent* EnhancedInputComponent);
};
