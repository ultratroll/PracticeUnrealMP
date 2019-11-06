// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMP_Definitions.h"
#include "BBQ_InteractionComponent.generated.h"


class UTexture2D;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BASICMP_API UBBQ_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	TArray<UPrimitiveComponent*> PrimitiveComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	TEnumAsByte<ECollisionChannel> InteractionChannel;

	// Sets default values for this component's properties
	UBBQ_InteractionComponent();

protected:

	// If enabled, will try to autoadd primitives
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bAutoAddPrimitives : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bCanInteract : 1;

	// Max distance the player is allowed to interact from.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxInteractDistance = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D * InteractionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractableName;

	// Populate if this interaction requires the use of item(s).
	// filled with items in this array.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TArray<FItemInteractionType>	PossibleItemInteractions;

	// Testing. Makes sure to bind each primitive.
	void SetupInteractionPrimitives();

protected:

	virtual void BeginPlay() override;

#if 0
	virtual void BeginDestroy() override;
#endif

	// Checks to see if any primitive components are explicitly defined.
	// If not, lets use the children of the owner.
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void CheckPrimitives();

	UFUNCTION()
	void SetCurrentInteractableString();

	// Get interactable elements
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	TArray<UPrimitiveComponent *> & GetPrimitiveComponents() { return PrimitiveComponents; }

	// Add interactable element
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void AddPrimitive(UPrimitiveComponent* PrimitiveComponent);

	UFUNCTION()
	void OnBeginOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsInteractionEnabled() const { return bCanInteract != 0; }

	UFUNCTION()
	void HandleBeginOverLapPrimitive(UPrimitiveComponent *TouchedComponent);

	UFUNCTION()
	void HandleEndOverLapPrimitive(UPrimitiveComponent *TouchedComponent);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if 0
	//TODO perhaps remove
	void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
#endif
};
