// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Slash/DebugMacros.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"


AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CollisionCapsule->SetupAttachment(GetRootComponent());

	NiagaraEmberEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Embers"));
	NiagaraEmberEffect->SetupAttachment(GetRootComponent());

}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	CollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnCollisionCapsuleOverlap);
	CollisionCapsule->OnComponentEndOverlap.AddDynamic(this, &AItem::OnCollisionCapsuleEndOverlap);


}

float AItem::TransformedSin()
{
	return SinAmplitude * FMath::Sin(RunningTime * SinSpeed);
}

float AItem::TransformedCos()
{
	return CosAmplitude * FMath::Cos(RunningTime * CosSpeed);
}


void AItem::OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);

	if (PickupInterface)
	{
		//UE_LOG(LogTemp, Warning, TEXT("START overlap with item named %s"), *GetName());
		PickupInterface ->SetOverlappingItem(this);
	}

}

void AItem::OnCollisionCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);

	if (PickupInterface)
	{
		//UE_LOG(LogTemp, Warning, TEXT("END overlap with item named %s"), *GetName());
		PickupInterface->SetOverlappingItem(nullptr);
	}

}

void AItem::PickupSystem(bool ShouldDestroy)
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation()
		);
	}

	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}

	if (ShouldDestroy)
	{
		//TODO remove this if using timer below
		Destroy();

		/*if (DestroyTimer <= 0)
		{
			Destroy();
		}
		else
		{
			SetLifeSpan(DestroyTimer);
		}*/
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

