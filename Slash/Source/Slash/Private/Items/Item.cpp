// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"
#include "NiagaraComponent.h"


AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	NiagaraEmberEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Embers"));
	NiagaraEmberEffect->SetupAttachment(GetRootComponent());

}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);


}

float AItem::TransformedSin()
{
	return SinAmplitude * FMath::Sin(RunningTime * SinSpeed);
}

float AItem::TransformedCos()
{
	return CosAmplitude * FMath::Cos(RunningTime * CosSpeed);
}


void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);

	if (SlashCharacter) 
	{
		SlashCharacter->SetOverlappingItem(this);
	}

}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);

	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(nullptr);
	}

}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}

	if (ItemState == EItemState::EIS_Rotating)
	{
		AddActorWorldRotation(FRotator(
			(RotationRateX * DeltaTime), (RotationRateY * DeltaTime), (RotationRateZ * DeltaTime)));
	}

	if (NiagaraEmberEffect)
	{
		if (!isParticleOn)
		{
			if (ParticleDelayTimer == 0)
			{
				NiagaraEmberEffect->Deactivate();
			}

			float DelayTimeEndDeltaTime = EffectDelayTimeEnd * DeltaTime;
			float ParticleDelayDeltaTime = ParticleDelayTimer * DeltaTime;

			if (ParticleDelayDeltaTime >= DelayTimeEndDeltaTime)
			{
				NiagaraEmberEffect->Activate();

				isParticleOn = true;
			}
			
			ParticleDelayTimer += 1;
		}
	}

}

