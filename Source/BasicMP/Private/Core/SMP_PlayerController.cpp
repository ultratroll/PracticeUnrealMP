// Fill out your copyright notice in the Description page of Project Settings.


#include "SMP_PlayerController.h"

void ASMP_PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bCanInteract != 0)
		UpdateClosestInteraction();
}

bool ASMP_PlayerController::UpdateClosestInteraction()
{
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
