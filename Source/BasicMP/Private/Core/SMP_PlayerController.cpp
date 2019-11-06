// Fill out your copyright notice in the Description page of Project Settings.


#include "SMP_PlayerController.h"
#include "Interaction/BBQ_InteractionComponent.h"

// -----------------------------------------------------------------------------------------
ASMP_PlayerController::ASMP_PlayerController() : Super()
{
	bCanInteract = true;
}

void ASMP_PlayerController::RegisterNearbyInteraction(UBBQ_InteractionComponent *InteractionComponent, UPrimitiveComponent* primitiveComponent)
{
	if (InteractionComponent != nullptr && primitiveComponent != nullptr)
	{
		const auto& foundInteractionPrimitive = OverlappedInteractionPrimitives.FindByPredicate([&](const FInteractionPrimitive& interactionPrimitive)
		{
			return interactionPrimitive.GetInteractionComponent() == InteractionComponent && interactionPrimitive.GetPrimitiveComponent() == primitiveComponent;
		});

		if (foundInteractionPrimitive == nullptr)
		{
			OverlappedInteractionPrimitives.Add(FInteractionPrimitive(InteractionComponent, primitiveComponent));
		}
	}
}

void ASMP_PlayerController::UnregisterNearbyInteraction(UBBQ_InteractionComponent *InteractionComponent, UPrimitiveComponent* primitiveComponent)
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
void ASMP_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bCanInteract != 0)
		UpdateClosestInteraction();
}

// -----------------------------------------------------------------------------------------
bool ASMP_PlayerController::UpdateClosestInteraction()
{
	if (IsInteractionEnabled() && OverlappedInteractionPrimitives.Num() > 0)
	{
		;
	}

	FHitResult TraceResult(ForceInit);

	// Viewport Size
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);

	FVector2D CrosshairPosition = FVector2D(ViewportSizeX / 2, ViewportSizeY / 2);

	bool bHit = GetHitResultAtScreenPosition(CrosshairPosition, InteractionChannel, false, TraceResult);

	if (TraceResult.GetActor() != nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("%s !"), *TraceResult.GetActor()->GetName());
	}
	if (TraceResult.GetComponent() != nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("%s !"), *TraceResult.GetComponent()->GetName());
	}

	return true;
}
