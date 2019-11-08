// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SMP_Definitions.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SMP_PlayerController.generated.h"

/**
 * 
 */
// class UBBQ_InteractionComponent;
// class UPrimitiveComponent;
// 
// // A pair of interaction and primitive
// USTRUCT()
// struct FInteractionPrimitive
// {
// 	GENERATED_USTRUCT_BODY()
// 
// private:
// 
// 	UBBQ_InteractionComponent* InteractionComponent; //TWeakObjectPtr<UBBQ_InteractionComponent> InteractionComponent;
// 	UPrimitiveComponent* PrimitiveComponent; //TWeakObjectPtr<UPrimitiveComponent> PrimitiveComponent;
// 
// public:
// 
// 	FInteractionPrimitive() : InteractionComponent(nullptr), PrimitiveComponent(nullptr) { }
// 
// 	FInteractionPrimitive(UBBQ_InteractionComponent* interactionComponent, UPrimitiveComponent* primitiveComponent)
// 		: InteractionComponent(interactionComponent), PrimitiveComponent(primitiveComponent) { }
// 
// 	UBBQ_InteractionComponent* GetInteractionComponent() const { return InteractionComponent; } // .Get();
// 	UPrimitiveComponent* GetPrimitiveComponent() const { return PrimitiveComponent; } // .Get();
// };

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

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	
	// Updates the current interaction content, making sure to choose the closest one
	bool UpdateClosestInteraction();
};
