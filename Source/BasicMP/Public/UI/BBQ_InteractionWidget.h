// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBQ_InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class BASICMP_API UBBQ_InteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Interaction")
	void BP_SetInteractionVisuals(const FText& Text, UTexture2D* Icon, bool InteractionEnabled = true, bool bUseHold= false, float CurrentHoldRatio= 0.0f);
};
