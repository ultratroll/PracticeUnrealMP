// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMP_PlayerController.generated.h"

/**
 * 
 */

class UBBQ_InteractionComponent;

UCLASS()
class BASICMP_API ASMP_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	TEnumAsByte<ECollisionChannel> InteractionChannel;

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Interactable")
	UBBQ_InteractionComponent* CurrentInteraction;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	
	// Updates the current interaction content, making sure to choose the closest one
	bool UpdateClosestInteraction();

private:
	uint8 bCanInteract = true;
};
