// Fill out your copyright notice in the Description page of Project Settings.


#include "BBQ_InteractAreaComponent.h"
#include "DrawDebugHelpers.h"
#include "UI/BBQ_InteractionWidget.h"
#include "Interaction/BBQ_InteractionComponent.h"
#include "Core/SMP_PlayerController.h"
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

	SetCollisionResponseToAllChannels(ECR_Overlap);
	//SetCollisionResponseToChannel(InteractionChannel, ECR_Overlap);
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
				Server_SetCurrentInteraction(nullptr);

				ASMP_PlayerController* const MyPC = Cast<ASMP_PlayerController>(GetOwner()->GetGameInstance()->GetFirstLocalPlayerController());
				if (MyPC)
				{
					// Turn off interaction UI
					UBBQ_InteractionWidget* InteractionUI = MyPC->InteractionUI;
					if (InteractionUI)
					{
						InteractionUI->SetVisibility(ESlateVisibility::Collapsed);
						InteractionUI->SetInteractionVisuals(FText::GetEmpty(), nullptr);
					}
				}
			}
			break;
		}
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::BeginPlay()
{
	Super::BeginPlay();

#if 0
	OnComponentBeginOverlap.AddDynamic(this, &UBBQ_InteractAreaComponent::OnBeginOverLapPrimitive);
	OnComponentEndOverlap.AddDynamic(this, &UBBQ_InteractAreaComponent::OnEndOverLapPrimitive);
#endif	
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
		// Perhaps save the player controller right away
		ASMP_PlayerController* const MyPC = Cast<ASMP_PlayerController>(GetOwner()->GetGameInstance()->GetFirstLocalPlayerController());
		
		if (MyPC)
		{
			TArray<FInteractionPrimitive> CurrentOverlappedInteractionPrimitives(OverlappedInteractionPrimitives);

			// Clean up
			CurrentOverlappedInteractionPrimitives.RemoveAll([&](const FInteractionPrimitive& interactionPrimitive)
			{
				return interactionPrimitive.GetInteractionComponent() == nullptr || interactionPrimitive.GetPrimitiveComponent() == nullptr;
			});

			FHitResult TraceResult(ForceInit);

			// Viewport Size
			int32 ViewportSizeX, ViewportSizeY;
			MyPC->GetViewportSize(ViewportSizeX, ViewportSizeY);

			FVector2D CrosshairPosition = FVector2D(ViewportSizeX / 2, ViewportSizeY / 2);

			bool bHit = MyPC->GetHitResultAtScreenPosition(CrosshairPosition, InteractionChannel, false, TraceResult);

			if (bHit && TraceResult.GetComponent())
			{
				// Check if the hit primitive is one of the overlapping interactions.
				const FInteractionPrimitive* ClosestInteractionPrimitive = OverlappedInteractionPrimitives.FindByPredicate([&](const FInteractionPrimitive& interactionPrimitive)
				{
					return interactionPrimitive.GetPrimitiveComponent() == TraceResult.GetComponent();
				});

				if (ClosestInteractionPrimitive != nullptr)
				{
					// TODO check when we REALLY need to update the ui
					if (CurrentInteraction != ClosestInteractionPrimitive->GetInteractionComponent())
					{
						Server_SetCurrentInteraction(ClosestInteractionPrimitive->GetInteractionComponent());
					}

					// Turn off interaction UI
					UBBQ_InteractionWidget* InteractionUI = MyPC->InteractionUI;
					if (InteractionUI)
					{
						InteractionUI->SetVisibility(ESlateVisibility::HitTestInvisible);
						InteractionUI->SetInteractionVisuals(ClosestInteractionPrimitive->GetInteractionComponent()->GetText(), ClosestInteractionPrimitive->GetInteractionComponent()->GetIcon());

						FVector2D ScreenLocation;
						MyPC->ProjectWorldLocationToScreen(ClosestInteractionPrimitive->GetPrimitiveComponent()->GetComponentLocation(), ScreenLocation);

						ScreenLocation += FVector2D(CrosshairPosition.X - ScreenLocation.X, CrosshairPosition.Y - ScreenLocation.Y);
						InteractionUI->SetPositionInViewport(ScreenLocation);
					}

					if (TraceResult.GetActor() != nullptr)
					{
						UE_LOG(LogActor, Warning, TEXT("SHOW UI FOR %s !"), *TraceResult.GetActor()->GetName());
					}
					if (TraceResult.GetComponent() != nullptr)
					{
						UE_LOG(LogActor, Warning, TEXT("SHOW UI FOR %s !"), *TraceResult.GetComponent()->GetName());
					}

					// TODO:: pass the current interaction to the player controller!
				}
			}
			else
			{
				UBBQ_InteractionWidget* InteractionUI = MyPC->InteractionUI;
				if (InteractionUI)
				{
					InteractionUI->SetVisibility(ESlateVisibility::Collapsed);
					InteractionUI->SetInteractionVisuals(FText::GetEmpty(), nullptr);
				}
			}
		}
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

void UBBQ_InteractAreaComponent::Server_SetCurrentInteraction_Implementation(UBBQ_InteractionComponent* NewInteraction)
{
	CurrentInteraction = NewInteraction;

	ASMP_PlayerController* const MyPC = Cast<ASMP_PlayerController>(GetOwner()->GetGameInstance()->GetFirstLocalPlayerController());
	if (MyPC)
	{
		MyPC->CurrentInteraction = CurrentInteraction;
	}
}

bool UBBQ_InteractAreaComponent::Server_SetCurrentInteraction_Validate(UBBQ_InteractionComponent* NewInteraction)
{
	return true;
}

#if 0
void UBBQ_InteractAreaComponent::OnBeginOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractAreaComponent::OnBeginOverLapPrimitive() - Owner: %s with %s"), *(GetOwner()->GetFullName()), *(OverlappedComponent != nullptr ? OtherComp->GetFullName() : "no component"));
	UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractAreaComponent::OnBeginOverLapPrimitive() - with %s"), *(OverlappedComponent != nullptr ? OtherComp->GetFullName() : "no component"));
}

void UBBQ_InteractAreaComponent::OnEndOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractAreaComponent::OnEndOverLapPrimitive() - Owner: %s with %s"), *(GetOwner()->GetFullName()), *(OverlappedComponent != nullptr ? OtherComp->GetFullName() : "no component"));
	UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractAreaComponent::OnEndOverLapPrimitive() - with %s"), *(OverlappedComponent != nullptr ? OtherComp->GetFullName() : "no component"));
}
#endif

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// TODO check when i finish the replication part
	//DOREPLIFETIME(UBBQ_InteractAreaComponent, OverlappedInteractionPrimitives);
	DOREPLIFETIME_CONDITION(UBBQ_InteractAreaComponent, OverlappedInteractionPrimitives, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UBBQ_InteractAreaComponent, CurrentInteraction, COND_OwnerOnly);
}

