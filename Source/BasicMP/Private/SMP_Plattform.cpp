// Fill out your copyright notice in the Description page of Project Settings.


#include "SMP_Plattform.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASMP_Plattform::ASMP_Plattform()
{
	PrimaryActorTick.bCanEverTick = true;

	SetMobility(EComponentMobility::Movable);

	Speed = 5.0f;
	bCanPatrol = true;
	CurrentPatrolIndex = -1;

}

// Called when the game starts or when spawned
void ASMP_Plattform::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);

		OriginalLocation = GetActorLocation();

		SetupPatrol(CurrentPatrolIndex);
	}	
}

// Called every frame
void ASMP_Plattform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		UpdatePatrol(DeltaTime);
	}
}

void ASMP_Plattform::UpdatePatrol(float DeltaTime)
{
	if (!bCanPatrol || PatrolPoints.Num() == 0)
	{
		return;
	}

	SetActorLocation(GetActorLocation() + DeltaTime * Speed * CurrentDirection);

	// Check if its time to change target
	if ((GetActorLocation() - StartLocation).Size() >= CurrentMaxDistance)
	{
		// For a single patrol point, nothing else to do, stop patrolling
		if (PatrolPoints.Num() == 1)
		{
			bCanPatrol = false;
			return;
		}

		CurrentPatrolIndex++;
		SetupPatrol(CurrentPatrolIndex);
	}

}

void ASMP_Plattform::SetupPatrol(int Index)
{

	CurrentPatrolIndex = Index;

	if (!bCanPatrol || PatrolPoints.Num() == 0)
	{
		return;
	}

	StartLocation = GetActorLocation();

	if (CurrentPatrolIndex >= PatrolPoints.Num() || CurrentPatrolIndex < 0)
	{
		CurrentPatrolIndex = 0;
	}

	EndLocation = OriginalLocation + PatrolPoints[CurrentPatrolIndex];

	CurrentDirection = EndLocation - StartLocation;

	DrawDebugSphere(GetWorld(), StartLocation, 5, 10, FColor::Blue, false, 2.0f);
	DrawDebugSphere(GetWorld(), EndLocation, 5, 10, FColor::Red, false, 2.0f);

	CurrentMaxDistance = CurrentDirection.Size();

	//UE_LOG(LogTemp, Warning, TEXT("Start Location is %s"), *StartLocation.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("End Location is %s"), *EndLocation.ToString());

	CurrentDirection.Normalize();
}

