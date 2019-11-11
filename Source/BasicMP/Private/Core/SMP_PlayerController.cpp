// Fill out your copyright notice in the Description page of Project Settings.


#include "SMP_PlayerController.h"
#include "Interaction/BBQ_InteractionComponent.h"

// -----------------------------------------------------------------------------------------
ASMP_PlayerController::ASMP_PlayerController() : Super()
{
	bCanInteract = true;
}

// -----------------------------------------------------------------------------------------
void ASMP_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (IsInteractionEnabled())
		UpdateTracing();
}

// -----------------------------------------------------------------------------------------
bool ASMP_PlayerController::UpdateTracing()
{
	// for the time being seems to work well
	if (bDebug ==0)
		return 0;

	FHitResult TraceResult(ForceInit);

	// Viewport Size
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);

	FVector2D CrosshairPosition = FVector2D(ViewportSizeX / 2, ViewportSizeY / 2);

	bool bHit = GetHitResultAtScreenPosition(CrosshairPosition, InteractionChannel, false, TraceResult);

	if (TraceResult.GetActor() != nullptr)
	{
		//UE_LOG(LogActor, Warning, TEXT("%s !"), *TraceResult.GetActor()->GetName());
	}
	if (TraceResult.GetComponent() != nullptr)
	{
		UE_LOG(LogActor, Warning, TEXT("ASMP_PlayerController::UpdateClosestInteraction--Comp %s !"), *TraceResult.GetComponent()->GetName());
	}

	return true;
}
