#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BBQ_InteractAreaComponent.generated.h"

class USphereComponent;
class UBBQ_InteractionComponent;
class UPrimitiveComponent;
class ASMP_PlayerController;
class UBBQ_InteractionWidget;

// A pair of interaction and primitive
USTRUCT(BlueprintType)
struct FInteractionPrimitive
{
	GENERATED_USTRUCT_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UBBQ_InteractionComponent* InteractionComponent; //TWeakObjectPtr<UBBQ_InteractionComponent> InteractionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UPrimitiveComponent* PrimitiveComponent; //TWeakObjectPtr<UPrimitiveComponent> PrimitiveComponent;

public:

	FInteractionPrimitive() : InteractionComponent(nullptr), PrimitiveComponent(nullptr) { }

	FInteractionPrimitive(UBBQ_InteractionComponent* interactionComponent, UPrimitiveComponent* primitiveComponent)
		: InteractionComponent(interactionComponent), PrimitiveComponent(primitiveComponent) { }

	UBBQ_InteractionComponent* GetInteractionComponent() const { return InteractionComponent; } // .Get();
	UPrimitiveComponent* GetPrimitiveComponent() const { return PrimitiveComponent; } // .Get();
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BASICMP_API UBBQ_InteractAreaComponent : public USphereComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UBBQ_InteractAreaComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	TEnumAsByte<ECollisionChannel> InteractionChannel;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool IsInteractionEnabled() const { return bCanInteract != 0; }
	
	UFUNCTION()
	void RegisterNearbyInteraction(UBBQ_InteractionComponent *InteractionComponent, UPrimitiveComponent* primitiveComponent);

	UFUNCTION()
	void UnregisterNearbyInteraction(UBBQ_InteractionComponent *InteractionComponent, UPrimitiveComponent* primitiveComponent);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UBBQ_InteractionComponent* GetCurrentInteraction() { return CurrentInteraction; }

#if 0
	void TryBeginInteraction();

	void TryEndInteraction();
#endif

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_TryBeginInteraction();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_TryEndInteraction();

protected:

	ASMP_PlayerController* MyPC;

	//UBBQ_InteractionWidget* InteractionUI;

	// Current interaction the player is looking at
	UPROPERTY(Transient, Replicated)
	UBBQ_InteractionComponent* CurrentInteraction;

	// Current interaction the player is actually interacting with
	UPROPERTY(Transient, Replicated)
	UBBQ_InteractionComponent* CurrentActiveInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bCanInteract : 1;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Interactable")
	uint8 bIsInteracting : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bDebug : 1;

	// overlapped interaction components.
	UPROPERTY(Transient, Replicated, EditAnywhere, BlueprintReadWrite)
	TArray<FInteractionPrimitive> OverlappedInteractionPrimitives;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Updates the current interaction content, making sure to choose the closest one
	bool UpdateClosestInteraction();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_SetCurrentInteraction(UBBQ_InteractionComponent* NewInteraction);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_SetCurrentActiveInteraction(UBBQ_InteractionComponent* NewInteraction);
	
	// Resets the current interaction and hides its UI
	void DisableCurrentInteraction();

#if 0
	// See if gonna leave or not
	UFUNCTION()
	void OnBeginOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// See if gonna leave or not
	UFUNCTION()
	void OnEndOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#endif

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetIsInteracting(bool bInteracting);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool GetIsInteracting() const { return bIsInteracting != 0; }
};
