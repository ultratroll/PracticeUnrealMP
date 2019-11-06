// Fill out your copyright notice in the Description page of Project Settings.


#include "BBQ_InteractAreaComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UBBQ_InteractAreaComponent::UBBQ_InteractAreaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

// 	UPrimitiveComponent* rootPrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
// 	if (rootPrimitiveComponent != nullptr)
// 	{
// 		PrimitiveComponents.Add(rootPrimitiveComponent);
// 	}

	InteractionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Component"));
	InteractionCollider->SetupAttachment(GetOwner()->GetRootComponent());
}


// Called when the game starts
void UBBQ_InteractAreaComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBBQ_InteractAreaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

