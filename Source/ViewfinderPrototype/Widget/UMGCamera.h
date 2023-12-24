// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScaleBox.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "UMGCamera.generated.h"

/**
 * 
 */
UCLASS()
class VIEWFINDERPROTOTYPE_API UUMGCamera : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UScaleBox* CameraBox;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UCanvasPanel* CameraPanel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* CameraImage;
};
