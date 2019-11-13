// Fill out your copyright notice in the Description page of Project Settings.


#include "BBQ_InteractionComponent.h"
#include "Engine/Texture2D.h"
#include "UnrealNetwork.h"
#include "Interaction/BBQ_InteractAreaComponent.h"
#include "Core/SMP_PlayerController.h" // change this to BBQ

// -----------------------------------------------------------------------------------------
UBBQ_InteractionComponent::UBBQ_InteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bReplicates = true;
	bAutoAddPrimitives = false;
	bCanInteract = true;
	bCheckForMaxDistance = true;
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::TryBeginInteraction()
{
	Server_TryBeginInteraction();
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::TryEndInteraction()
{
	Server_TryEndInteraction();
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::Server_TryBeginInteraction_Implementation()
{
	if (IsInteractionEnabled() && !IsInteracting())
	{
		bIsInteracting = true;

		OnInteractionForServerDelegate.Broadcast(bIsInteracting);

		// TODO: Removed temporaly, Netmulticast to clients
	}
}

// -----------------------------------------------------------------------------------------
bool UBBQ_InteractionComponent::Server_TryBeginInteraction_Validate()
{
	return true;
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::Server_TryEndInteraction_Implementation()
{
	if (IsInteractionEnabled() && IsInteracting())
	{
		bIsInteracting = false;

		OnInteractionForServerDelegate.Broadcast(bIsInteracting);

		// TODO: Removed temporaly, Netmulticast to clients
	}
}

// -----------------------------------------------------------------------------------------
bool UBBQ_InteractionComponent::Server_TryEndInteraction_Validate()
{
	return true;
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::SetupInteractionPrimitives()
{
	// Testing if needed
	for (auto & PrimitiveComponent : PrimitiveComponents)
	{
		if (PrimitiveComponent == nullptr)
		{
			UE_LOG(LogActor, Warning, TEXT("%s has provided the InteractionComponent with a null Primitive!"), *GetOwner()->GetFullName());
			continue;
		}

		if (GetOwnerRole() == ROLE_Authority)
		{
			PrimitiveComponent->SetGenerateOverlapEvents(true);
			PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UBBQ_InteractionComponent::OnBeginOverLapPrimitive);
			PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UBBQ_InteractionComponent::OnEndOverLapPrimitive);
			PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &UBBQ_InteractionComponent::OnBeginOverLapPrimitive);
			PrimitiveComponent->OnComponentEndOverlap.AddDynamic(this, &UBBQ_InteractionComponent::OnEndOverLapPrimitive);
		}
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckPrimitives();
}


#if 0 
// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::BeginDestroy()
{
	for (auto & PrimitiveComponent : PrimitiveComponents)
	{
		if (PrimitiveComponent == nullptr)
		{
			UE_LOG(LogActor, Warning, TEXT("%s has provided the InteractionComponent with a null Primitive!"), *GetOwner()->GetFullName());
			continue;
		}

		PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UBBQ_InteractionComponent::OnBeginOverLapPrimitive);
		PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UBBQ_InteractionComponent::OnEndOverLapPrimitive);
	}
}
#endif

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::CheckPrimitives()
{
	// Auto find primitives if they aren't explicitly defined.
	if (PrimitiveComponents.Num() == 0 && bAutoAddPrimitives != 0)
	{
		UPrimitiveComponent* rootPrimitiveComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
		if (rootPrimitiveComponent != nullptr)
		{
			PrimitiveComponents.Add(rootPrimitiveComponent);
		}

		TArray<USceneComponent*> childrenComponents;
		GetOwner()->GetRootComponent()->GetChildrenComponents(true, childrenComponents);
		UPrimitiveComponent* primitiveComponent;
		for (int i = 0; i < childrenComponents.Num(); i++)
		{
			primitiveComponent = Cast<UPrimitiveComponent>(childrenComponents[i]);
			if (primitiveComponent != nullptr)
			{
				if (GetOwner() == primitiveComponent->GetOwner())
				{
					PrimitiveComponents.Add(primitiveComponent);
					primitiveComponent->SetCollisionResponseToChannel(InteractionChannel, ECR_Block);
				}
			}
		}
	}
	else
	{
		// if there are some primitives assigned already, someone may have passed in bad data, verify just in case.
		for (int i = PrimitiveComponents.Num() - 1; i >= 0; i--)
		{
			if (PrimitiveComponents[i] == nullptr)
			{
				UE_LOG(LogActor, Warning, TEXT("%s has a InteractionComponent that's been given a null Primitive!"), *GetOwner()->GetFullName());
				PrimitiveComponents.RemoveAt(i);
			}
			else
			{
				PrimitiveComponents[i]->SetCollisionResponseToChannel(InteractionChannel, ECR_Block);
			}
		}

		// Filter the list to only contains unique primitives.
		TArray<UPrimitiveComponent*> uniquePrimitives;
		for (auto* primitive : PrimitiveComponents)
		{
			uniquePrimitives.AddUnique(primitive);
		}

		PrimitiveComponents = uniquePrimitives;
	}

#if 0
	// Store the default custom render depth states
	for (const auto& primitive : PrimitiveComponents)
	{
		DefaultRenderCustomDepth.Add(primitive->bRenderCustomDepth);
	}
#endif

#if 0
	SetupInteractionPrimitives();
#endif
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::SetCurrentInteractableString()
{
	// Change the string if needed
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::AddPrimitive(UPrimitiveComponent* PrimitiveComponent)
{
	if (IsValid(PrimitiveComponent))
	{
		PrimitiveComponents.AddUnique(PrimitiveComponent);
		PrimitiveComponent->SetCollisionResponseToChannel(InteractionChannel, ECR_Block);
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UBBQ_InteractionComponent::OnBeginOverLapPrimitive);
		PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UBBQ_InteractionComponent::OnEndOverLapPrimitive);
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &UBBQ_InteractionComponent::OnBeginOverLapPrimitive);
		PrimitiveComponent->OnComponentEndOverlap.AddDynamic(this, &UBBQ_InteractionComponent::OnEndOverLapPrimitive);
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#if 0 //WITH_EDITOR
// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(UBBQ_InteractionComponent, PrimitiveComponents)) || (PropertyName == GET_MEMBER_NAME_CHECKED(UBBQ_InteractionComponent, PrimitiveComponents)))
	{
		if (PrimitiveComponents.Num() > 0)
		{
			for (auto* primitive : PrimitiveComponents)
			{
				if (IsValid(primitive))
					primitive->SetCollisionResponseToChannel(InteractionChannel, ECR_Block);

				bAutoAddPrimitives = false;
			}
		}
	}
}

#endif

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::OnBeginOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractionComponent::OnBeginOverLapPrimitive() - Owner: %s "), *(GetOwner()->GetFullName()));
	//UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractionComponent::OnBeginOverLapPrimitive() - with %s"), *(OverlappedComponent != nullptr ? OtherComp->GetFullName() : "no component"));

	UBBQ_InteractAreaComponent* InteractableArea = Cast<UBBQ_InteractAreaComponent>(OtherComp);
	
	if (InteractableArea)
	{
		SetCurrentInteractableString();
		InteractableArea->RegisterNearbyInteraction(this, OverlappedComponent);
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::OnEndOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractionComponent::OnEndOverLapPrimitive() - Owner: %s "), *(GetOwner()->GetFullName()));
	//UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractionComponent::OnEndOverLapPrimitive() - with %s"), *(OverlappedComponent != nullptr ? OtherComp->GetFullName() : "no component"));

	UBBQ_InteractAreaComponent* InteractableArea = Cast<UBBQ_InteractAreaComponent>(OtherComp);

	if (InteractableArea)
	{
		InteractableArea->UnregisterNearbyInteraction(this, OverlappedComponent);
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBBQ_InteractionComponent, bCanInteract);
	DOREPLIFETIME(UBBQ_InteractionComponent, bIsInteracting);
}

