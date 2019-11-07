// Fill out your copyright notice in the Description page of Project Settings.


#include "BBQ_InteractAreaComponent.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"
#include "Components/SphereComponent.h"

// -----------------------------------------------------------------------------------------
UBBQ_InteractAreaComponent::UBBQ_InteractAreaComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	SphereRadius = 250.0f;

	bCanInteract = true;
	bReplicates = true;
	bAutoActivate = true;

	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(InteractionChannel, ECR_Overlap);
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::RegisterNearbyInteraction(UBBQ_InteractionComponent *InteractionComponent, UPrimitiveComponent* primitiveComponent)
{
	if (InteractionComponent != nullptr && primitiveComponent != nullptr)
	{
		const auto& foundInteractionPrimitive = OverlappedInteractionPrimitives.FindByPredicate([&](const FInteractionPrimitive& interactionPrimitive)
		{
			return interactionPrimitive.GetInteractionComponent() == InteractionComponent && interactionPrimitive.GetPrimitiveComponent() == primitiveComponent;
		});

		// If has not been added
		if (foundInteractionPrimitive == nullptr)
		{
			OverlappedInteractionPrimitives.Add(FInteractionPrimitive(InteractionComponent, primitiveComponent));
		}
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::UnregisterNearbyInteraction(UBBQ_InteractionComponent *InteractionComponent, UPrimitiveComponent* primitiveComponent)
{
	for (int i = 0; i < OverlappedInteractionPrimitives.Num(); ++i)
	{
		const FInteractionPrimitive& interactionPrimitive = OverlappedInteractionPrimitives[i];

		if (interactionPrimitive.GetInteractionComponent() == InteractionComponent
			&& interactionPrimitive.GetPrimitiveComponent() == primitiveComponent)
		{
			OverlappedInteractionPrimitives.RemoveAt(i);

			// Remove the UI if this is the last one in the list.
			if (OverlappedInteractionPrimitives.Num() <= 0)
			{
				CurrentInteraction = nullptr;

#if 0
				// Turn off interaction UI
				UBBQ_InteractionWidget* InteractionUI = ;
				InteractionUI->SetVisibility(ESlateVisibility::Collapsed);
				InteractionUI->SetInteractionVisuals(FText::GetEmpty(), nullptr, nullptr, -1, false);

#endif
			}
			break;
		}
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsInteractionEnabled())
		UpdateClosestInteraction();
}

// -----------------------------------------------------------------------------------------
bool UBBQ_InteractAreaComponent::UpdateClosestInteraction()
{
	if (IsInteractionEnabled() && OverlappedInteractionPrimitives.Num() > 0)
	{
		;
	}

// 	FHitResult TraceResult(ForceInit);
// 
// 	// Viewport Size
// 	int32 ViewportSizeX, ViewportSizeY;
// 	GetViewportSize(ViewportSizeX, ViewportSizeY);
// 
// 	FVector2D CrosshairPosition = FVector2D(ViewportSizeX / 2, ViewportSizeY / 2);
// 
// 	bool bHit = GetHitResultAtScreenPosition(CrosshairPosition, InteractionChannel, false, TraceResult);
// 
// 	if (TraceResult.GetActor() != nullptr)
// 	{
// 		UE_LOG(LogActor, Warning, TEXT("%s !"), *TraceResult.GetActor()->GetName());
// 	}
// 	if (TraceResult.GetComponent() != nullptr)
// 	{
// 		UE_LOG(LogActor, Warning, TEXT("%s !"), *TraceResult.GetComponent()->GetName());
// 	}

	return true;
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// TODO check when i finish the replication part
	//DOREPLIFETIME(UBBQ_InteractAreaComponent, OverlappedInteractionPrimitives);
	DOREPLIFETIME_CONDITION(UBBQ_InteractAreaComponent, OverlappedInteractionPrimitives, COND_OwnerOnly);

}

