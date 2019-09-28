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
	FVector Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Plattform", meta = (MakeEditWidget= true))
	FVector TargetLocation;

public:
	ASMPStaticMeshActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
