// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SMP_Definitions.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMP_PlayerController.generated.h"

class UBBQ_InteractionComponent;

UCLASS()
class BASICMP_API ASMP_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASMP_PlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	TEnumAsByte<ECollisionChannel> InteractionChannel;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsInteractionEnabled() const { return bCanInteract != 0; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bCanInteract : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bDebug : 1;

	UPROPERTY(BlueprintReadWrite, Category = "Interactable")
	UBBQ_InteractionComponent* CurrentInteractable;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	
	// Updates the current interaction content, making sure to choose the closest one
	bool UpdateClosestInteraction();
};
