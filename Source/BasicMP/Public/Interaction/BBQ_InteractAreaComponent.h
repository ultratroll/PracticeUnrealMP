#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BBQ_InteractAreaComponent.generated.h"

class USphereComponent;
class UBBQ_InteractionComponent;
class UPrimitiveComponent;

// A pair of interaction and primitive
USTRUCT()
struct FInteractionPrimitive
{
	GENERATED_USTRUCT_BODY()

private:

	UBBQ_InteractionComponent* InteractionComponent; //TWeakObjectPtr<UBBQ_InteractionComponent> InteractionComponent;
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

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bCanInteract : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	uint8 bDebug : 1;

	// overlapped interaction components.
	UPROPERTY(Transient, Replicated)
	TArray<FInteractionPrimitive> OverlappedInteractionPrimitives;

	UPROPERTY(BlueprintReadWrite, Category = "Interactable")
	UBBQ_InteractionComponent* CurrentInteraction;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Updates the current interaction content, making sure to choose the closest one
	bool UpdateClosestInteraction();

	UFUNCTION()
	void OnBeginOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
