// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMP_Definitions.h"
#include "BBQ_InteractionComponent.generated.h"


class UTexture2D;
class ABasicMPCharacter;

/** Delegate to report whenever an interactable begins or ends interaction. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBBQ_OnInteraction, bool, bIsInteracting);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BASICMP_API UBBQ_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	TArray<UPrimitiveComponent*> PrimitiveComponents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	TEnumAsByte<ECollisionChannel> InteractionChannel;

	/** To be called when the interaction begins, only called on server (stuff that should happen on all machines, like the movement of a door opening or closing). */
	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FBBQ_OnInteraction OnInteractionForServerDelegate;

	/** To be called when the interaction begins, only called on client (stuff like showing an vfx is only intended to be seen by the player that interacted). */
	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FBBQ_OnInteraction OnInteractionForClientDelegate;

	/** To be called when the interaction is openable and its open state changes, only called on client (for stuff like opening the door). */
	UPROPERTY(BlueprintAssignable, Category = "Interactable")
	FBBQ_OnInteraction OnInteractionForOpenableDelegate;

	// Sets default values for this component's properties
	UBBQ_InteractionComponent();

	UTexture2D* GetIcon() const { return InteractionIcon; }

	FText GetText() const { return InteractableName; }

	float GetMaxInteractDistance() const { return MaxInteractDistance; }

	float ShouldCheckForMaxDistance() const { return bCheckForMaxDistance; }

	int GetRequiredTeam() const { return RequiredTeam; }

	UFUNCTION(BlueprintCallable)
	void TryBeginInteraction(ABasicMPCharacter* NewInstigator);

	UFUNCTION(BlueprintCallable)
	void TryEndInteraction();

protected:

	// If enabled, will try to autoadd primitives
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interactable")
	uint8 bAutoAddPrimitives : 1;

	UPROPERTY(Replicated)
	uint8 bCanInteract : 1;

	UPROPERTY(Replicated)
	uint8 bIsInteracting : 1;

	/** If true, this interactable is openable, and as such, saves its openable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interactable|Settings")
	uint8 bIsOpenable : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interactable|Settings")
	uint8 bHoldToUse : 1;

	UPROPERTY(Replicated, Transient)
	uint8 bIsBeingHold : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bShowUIWhenInactive : 1;

	// If true, the interactive checks for the max distance, by default true
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interactable")
	uint8 bCheckForMaxDistance : 1;

	/** If true, this object has been opened at least once. Only matters if opening is enabled. */
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsOpen, meta = (editcondition = "bIsOpenable"))
	uint8 bIsOpen : 1;

	// Max distance the player is allowed to interact from.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxInteractDistance = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interactable|Settings")
	float HoldTime = 4.0f;

	UPROPERTY(Replicated, Transient)
	float CurrentHoldTime;

	UPROPERTY(Replicated, BlueprintReadWrite)
	ABasicMPCharacter* Instigator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D * InteractionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText InteractableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int RequiredTeam;

	// Populate if this interaction requires the use of item(s).
	// TODO: filled with items in this array. Turned off for the moment since its not critical for the test build, will add to BBQ soon.
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TArray<FItemInteractionType>	PossibleItemInteractions;

	// Testing. Makes sure to bind each primitive.
	void SetupInteractionPrimitives();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_TryBeginInteraction(ABasicMPCharacter* NewInstigator);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_TryEndInteraction();

	// Makes sure the interaction is no longer interacting
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_ResetInteraction();

	/** To be called whenever the bIsOpen value changes, so that the openable object reflects its new state. */
	UFUNCTION()
	virtual void OnRep_IsOpen();

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
	bool CanShowUIWhenInactive() const { return bShowUIWhenInactive != 0; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsInteractionEnabled() const { return bCanInteract != 0; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsInteracting() const { return bIsInteracting != 0; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsBeingHold() const { return bIsBeingHold != 0; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	float GetCurrentHoldTime() const { return CurrentHoldTime; }

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	float GetHoldTime() const { return HoldTime; }
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void BP_SetInteractionEnabled(bool bEnabled) { bCanInteract = bEnabled; } // TODO : Shouldnt be BP_ !!

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	ABasicMPCharacter* GetInstigator();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if 0
	//TODO perhaps remove
	void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
#endif
};
