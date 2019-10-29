// Fill out your copyright notice in the Description page of Project Settings.


#include "SMP_PlattformTrigger.h"
#include "components/BoxComponent.h"

// Sets default values
ASMP_PlattformTrigger::ASMP_PlattformTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = Root;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>("TriggerVolume");

	TriggerVolume->SetBoxExtent(FVector(100,100,100));

	TriggerVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	TriggerVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ASMP_PlattformTrigger::OnComponentBeginOverlap);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ASMP_PlattformTrigger::OnComponentEndOverlap);
	TriggerVolume->RegisterComponent();

}

// Called when the game starts or when spawned
void ASMP_PlattformTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASMP_PlattformTrigger::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Begin
}

void ASMP_PlattformTrigger::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//End
}

