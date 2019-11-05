// Fill out your copyright notice in the Description page of Project Settings.


#include "BBQ_InteractionComponent.h"
#include "Engine/Texture2D.h"

// -----------------------------------------------------------------------------------------
UBBQ_InteractionComponent::UBBQ_InteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bAutoAddPrimitives = true;
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	CheckPrimitives();

	// Testing
	for (auto & PrimitiveComponent : PrimitiveComponents)
	{
		if (PrimitiveComponent == nullptr)
		{
			UE_LOG(LogActor, Warning, TEXT("%s has provided the InteractionComponent with a null Primitive!"), *GetOwner()->GetFullName());

			continue;
		}

		PrimitiveComponent->SetGenerateOverlapEvents(true);
		PrimitiveComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UBBQ_InteractionComponent::OnBeginOverLapPrimitive);
		PrimitiveComponent->OnComponentEndOverlap.RemoveDynamic(this, &UBBQ_InteractionComponent::OnEndOverLapPrimitive);
		PrimitiveComponent->OnComponentBeginOverlap.AddDynamic(this, &UBBQ_InteractionComponent::OnBeginOverLapPrimitive);
		PrimitiveComponent->OnComponentEndOverlap.AddDynamic(this, &UBBQ_InteractionComponent::OnEndOverLapPrimitive);
	}
}

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
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::AddPrimitive(UPrimitiveComponent* PrimitiveComponent)
{
	if (IsValid(PrimitiveComponent))
	{
		PrimitiveComponents.AddUnique(PrimitiveComponent);
		PrimitiveComponent->SetCollisionResponseToChannel(InteractionChannel, ECR_Block);
	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#if WITH_EDITOR
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
	UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractionComponent::OnBeginOverLapPrimitive() - Owner: %s"), *(GetOwner()->GetFullName()));
	
// 	if (GetWorld() != nullptr)
// 	{
// 		AMyGameMode *gamemode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
// 		if (gamemode != nullptr)
// 		{
// 			A_PlayerPartyController* Controller = gamemode->GetPlayerController();
// 			A_PlayerPawn* PlayerPawn = Controller != nullptr ? Controller->GetPlayerPawn() : nullptr;
// 
// 			if (PlayerPawn != nullptr && OtherComp == PlayerPawn->GetInteractionCollider()) // && ! Controller IsPlayerInUIOrPauseMode ?
// 			{
// 				// Only call this in the case of InteractionCollider on PlayerPawn
// 				HandleBeginMouseCursorOver(OverlappedComponent);
// 			}
// 			// Always broadcast overlap began - could be SOEComponent overlap
// 			UBBQ_GlobalEventHandler * globalEventHandler = UBBQ_GameLibrary::GetGlobalEventHandler(this);
// 			globalEventHandler->OnInteractionComponentBeginOverlap.Broadcast(OtherComp, this);
// 		}
// 	}
}

// -----------------------------------------------------------------------------------------
void UBBQ_InteractionComponent::OnEndOverLapPrimitive(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogActor, Warning, TEXT("UBBQ_InteractionComponent::OnEndOverLapPrimitive() - Owner: %s"), *(GetOwner()->GetFullName()));
// 	if (GetWorld() != nullptr)
// 	{
// 		AMyGameMode *gamemode = Cast<AMyGameMode>(GetWorld()->GetAuthGameMode());
// 		if (gamemode != nullptr)
// 		{
// 			A_PlayerPawn* A_PlayerPawn = gamemode->GetPlayerController()->GetPlayerPawn();
// 
// 			if (A_PlayerPawn != nullptr && OtherComp == A_PlayerPawn->GetInteractionCollider())
// 			{
// 				// Only call this in the case of InteractionCollider on PlayerPawn
// 				HandleEndMouseCursorOver(OverlappedComponent);
// 			}
// 			// Always broadcast overlap ended 
// 			UBBQ_GlobalEventHandler * globalEventHandler = UBBQ_GameLibrary::GetGlobalEventHandler(this);
// 			globalEventHandler->OnInteractionComponentEndOverlap.Broadcast(OtherComp, this);
// 		}
// 	}
}

