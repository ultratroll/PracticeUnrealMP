// Fill out your copyright notice in the Description page of Project Settings.


#include "BBQ_InteractAreaComponent.h"
#include "DrawDebugHelpers.h"
#include "UI/BBQ_InteractionWidget.h"
#include "Interaction/BBQ_InteractionComponent.h"
#include "Core/SMP_PlayerController.h"
#include "UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "BasicMPCharacter.h"

// -----------------------------------------------------------------------------------------
UBBQ_InteractAreaComponent::UBBQ_InteractAreaComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	SphereRadius = 500.0f;

	bCanInteract = true;
	bReplicates = true;
	bAutoActivate = true;

	SetCollisionResponseToAllChannels(ECR_Overlap);
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

				UBBQ_InteractionWidget* InteractionUI = (MyPC) ? MyPC->InteractionUI : nullptr;
				if (InteractionUI)
				{
					InteractionUI->SetVisibility(ESlateVisibility::Collapsed);
					InteractionUI->BP_SetInteractionVisuals(FText::GetEmpty(), nullptr);
				}
			}
			break;
		}
	}
}

#if 0
// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::TryBeginInteraction()
{
	if (IsValid(CurrentInteraction))
	{
		UE_LOG(LogTemp, Warning, TEXT("UBBQ_InteractAreaComponent::TryBeginInteraction- Try interact with %s"), *CurrentInteraction->GetName());
		CurrentInteraction->TryBeginInteraction();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UBBQ_InteractAreaComponent::TryBeginInteraction- No interactable!"));
	}
}

void UBBQ_InteractAreaComponent::TryEndInteraction()
{
	if (IsValid(CurrentInteraction))
		CurrentInteraction->TryEndInteraction();
	else
		UE_LOG(LogTemp, Warning, TEXT("UBBQ_InteractAreaComponent::TryBeginInteraction- No interactable!"));
}
#endif

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::BeginPlay()
{
	Super::BeginPlay();

	MyPC = Cast<ASMP_PlayerController>(GetOwner()->GetGameInstance()->GetFirstLocalPlayerController());

#if 0
	OnComponentBeginOverlap.AddDynamic(this, &UBBQ_InteractAreaComponent::OnBeginOverLapPrimitive);
	OnComponentEndOverlap.AddDynamic(this, &UBBQ_InteractAreaComponent::OnEndOverLapPrimitive);
#endif	
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsInteractionEnabled() && MyPC && MyPC->IsLocalController())
		UpdateClosestInteraction();
}

void UBBQ_InteractAreaComponent::SetIsInteracting(bool bInteracting)
{
	bIsInteracting= bInteracting;
}

// -----------------------------------------------------------------------------------------
bool UBBQ_InteractAreaComponent::UpdateClosestInteraction()
{
	if (IsInteractionEnabled() && OverlappedInteractionPrimitives.Num() > 0)
	{	
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

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredComponent(this);
			QueryParams.AddIgnoredActor(GetOwner());

			bool bHit = MyPC->GetHitResultAtScreenPosition(CrosshairPosition, InteractionChannel, QueryParams, TraceResult);

			if (bHit && TraceResult.GetComponent())
			{
				// Check if the hit primitive is one of the overlapping interactions.
				const FInteractionPrimitive* ClosestInteractionPrimitive = OverlappedInteractionPrimitives.FindByPredicate([&](const FInteractionPrimitive& InteractionPrimitive)
				{
					UBBQ_InteractionComponent* InteractionCandidate= InteractionPrimitive.GetInteractionComponent();
					
					bool bInsideDistance = (InteractionCandidate->ShouldCheckForMaxDistance())?
						InteractionCandidate->GetMaxInteractDistance() >= FVector::Distance(GetOwner()->GetActorLocation(), InteractionPrimitive.GetPrimitiveComponent()->GetComponentLocation()) : true;

					// Also check if its inside the interaction area radius
					bInsideDistance = (bInsideDistance)? (SphereRadius >= InteractionCandidate->GetMaxInteractDistance()) : false;

					/*
					// We are gonna get the player team here from the player state !
					AShooterPlayerState* MyPlayerState = Cast<AShooterPlayerState>(GetPlayerState());
					if (MyPlayerState != NULL)
					{
						// Team
						int Team = (float)MyPlayerState->GetTeamNum();
					}
					*/
					// We are gonna save it in the player controller for testing only
					bool bCorrectTeam = InteractionCandidate->GetRequiredTeam() == MyPC->Team;

					return (InteractionPrimitive.GetPrimitiveComponent() == TraceResult.GetComponent()) && bInsideDistance && bCorrectTeam;
				});

				if (ClosestInteractionPrimitive != nullptr)
				{
					// Update it only it can be used
					if (IsInteractionEnabled() && ClosestInteractionPrimitive->GetInteractionComponent()->IsInteractionEnabled())
						Server_SetCurrentInteraction(ClosestInteractionPrimitive->GetInteractionComponent());

					//UE_LOG(LogActor, Warning, TEXT("DISTANCE! %f !"), FVector::Distance(GetOwner()->GetActorLocation(), ClosestInteractionPrimitive->GetPrimitiveComponent()->GetComponentLocation()));
					
					UBBQ_InteractionWidget* InteractionUI = (MyPC) ? MyPC->InteractionUI : nullptr;
					if (InteractionUI)
					{
						bool bShowUIAsInteractive = (ClosestInteractionPrimitive->GetInteractionComponent()->IsInteractionEnabled())?
							true: ClosestInteractionPrimitive->GetInteractionComponent()->CanShowUIWhenInactive();

						// Only matters to show the progress bar if this area belongs to the instigator
						bool bShouldShowHoldProgress = ClosestInteractionPrimitive->GetInteractionComponent()->IsBeingHold() && GetIsInteracting();

						float CurrentProgress = 0.0f;

						if (bShouldShowHoldProgress)
							CurrentProgress = ClosestInteractionPrimitive->GetInteractionComponent()->GetCurrentHoldTime() / ClosestInteractionPrimitive->GetInteractionComponent()->GetHoldTime();
						
						InteractionUI->SetVisibility(ESlateVisibility::HitTestInvisible);
						InteractionUI->BP_SetInteractionVisuals(ClosestInteractionPrimitive->GetInteractionComponent()->GetText(), ClosestInteractionPrimitive->GetInteractionComponent()->GetIcon(), bShowUIAsInteractive, bShouldShowHoldProgress, CurrentProgress);

						FVector2D ScreenLocation;
						MyPC->ProjectWorldLocationToScreen(ClosestInteractionPrimitive->GetPrimitiveComponent()->GetComponentLocation(), ScreenLocation);

						ScreenLocation += FVector2D(CrosshairPosition.X - ScreenLocation.X, CrosshairPosition.Y - ScreenLocation.Y);
						InteractionUI->SetPositionInViewport(ScreenLocation);
					}
				}
				else 
				{
					DisableCurrentInteraction();
				}
			}
			else
			{
				DisableCurrentInteraction();
			}
		}
	}

	return true;
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::DisableCurrentInteraction()
{
	UBBQ_InteractionWidget* InteractionUI = (MyPC) ? MyPC->InteractionUI : nullptr;
	if (InteractionUI)
	{
		InteractionUI->SetVisibility(ESlateVisibility::Collapsed);
		InteractionUI->BP_SetInteractionVisuals(FText::GetEmpty(), nullptr);
		if (IsInteractionEnabled())
			Server_SetCurrentInteraction(nullptr);
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::Server_TryEndInteraction_Implementation()
{
	if (IsValid(CurrentInteraction))
	{
		CurrentInteraction->TryEndInteraction();
		UE_LOG(LogTemp, Warning, TEXT("UBBQ_InteractAreaComponent::Server_TryEndInteraction- Interactable to try ending!"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("UBBQ_InteractAreaComponent::Server_TryEndInteraction- No interactable to try ending!"));
}

// -----------------------------------------------------------------------------------------
bool UBBQ_InteractAreaComponent::Server_TryEndInteraction_Validate()
{
	return true;
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::Server_TryBeginInteraction_Implementation()
{
	if (IsValid(CurrentInteraction))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UBBQ_InteractAreaComponent::TryBeginInteraction- Try interact with %s"), *CurrentInteraction->GetName());
		CurrentInteraction->TryBeginInteraction(Cast<ABasicMPCharacter>(GetOwner()));
	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("UBBQ_InteractAreaComponent::TryBeginInteraction- No interactable!"));
// 	}
}

// -----------------------------------------------------------------------------------------
bool UBBQ_InteractAreaComponent::Server_TryBeginInteraction_Validate()
{
	return true;
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::Server_SetCurrentInteraction_Implementation(UBBQ_InteractionComponent* NewInteraction)
{
	CurrentInteraction = NewInteraction;
}

// -----------------------------------------------------------------------------------------
bool UBBQ_InteractAreaComponent::Server_SetCurrentInteraction_Validate(UBBQ_InteractionComponent* NewInteraction)
{
	return true;
}

#if 0
// -----------------------------------------------------------------------------------------
void UBBQ_InteractAreaComponent::OnBeginOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractAreaComponent::OnBeginOverLapPrimitive() - Owner: %s with %s"), *(GetOwner()->GetFullName()), *(OverlappedComponent != nullptr ? OtherComp->GetFullName() : "no component"));
	UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractAreaComponent::OnBeginOverLapPrimitive() - with %s"), *(OverlappedComponent != nullptr ? OtherComp->GetFullName() : "no component"));
}

// -----------------------------------------------------------------------------------------
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

	DOREPLIFETIME_CONDITION(UBBQ_InteractAreaComponent, OverlappedInteractionPrimitives, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UBBQ_InteractAreaComponent, CurrentInteraction, COND_OwnerOnly);
	DOREPLIFETIME(UBBQ_InteractAreaComponent, bIsInteracting);
}

