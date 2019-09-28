// Fill out your copyright notice in the Description page of Project Settings.


#include "SMPStaticMeshActor.h"

ASMPStaticMeshActor::ASMPStaticMeshActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetMobility(EComponentMobility::Movable);

	Speed = FVector(5.0f,0.0f,0.0f);

	TargetLocation = FVector(400.0f, 0.0f, 0.0f);
}

void ASMPStaticMeshActor::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void ASMPStaticMeshActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FVector Location = GetActorLocation();
		Location += DeltaTime * Speed;

		//UE_LOG(LogTemp, Warning, TEXT("This is a message to yourself during runtime!"));

		SetActorLocation(Location);
	}
}
