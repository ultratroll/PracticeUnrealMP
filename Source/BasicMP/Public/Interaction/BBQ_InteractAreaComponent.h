#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BBQ_InteractAreaComponent.generated.h"

class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BASICMP_API UBBQ_InteractAreaComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBBQ_InteractAreaComponent();

protected:

	USphereComponent* InteractionCollider;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
