// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SMP_Plattform.generated.h"

UCLASS()
class BASICMP_API ASMP_Plattform : public AStaticMeshActor
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
	// Sets default values for this actor's properties
	ASMP_Plattform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
