// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SMPStaticMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class BASICMP_API ASMPStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()

protected:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Plattform")
	uint8 bCanPatrol : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Plattform")
	int CurrentPatrolIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Plattform")
	float Speed;

	UPROPERTY(Transient)
	FVector CurrentDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Plattform", meta = (MakeEditWidget = true))
	TArray<FVector> PatrolPoints;

private:

	UPROPERTY()
	float CurrentMaxDistance;

	UPROPERTY()
	FVector OriginalLocation;
	FVector StartLocation;
	FVector EndLocation;

	void UpdatePatrol(float DeltaTime);

	void SetupPatrol(int Index);

public:
	
	ASMPStaticMeshActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
