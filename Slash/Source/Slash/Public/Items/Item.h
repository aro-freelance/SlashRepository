// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Rotating,
	EIS_Held,
	EIS_Sheathed
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trig Parameters")
	float SinAmplitude = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trig Parameters")
	float SinSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trig Parameters")
	float CosAmplitude = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trig Parameters")
	float CosSpeed = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationRateX = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationRateY = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float RotationRateZ = 0.f;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnCollisionCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCollisionCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

	virtual void PickupSystem(bool ShouldDestroy);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;


	EItemState ItemState = EItemState::EIS_Hovering;
	
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CollisionCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara Effects")
	UNiagaraComponent* NiagaraEmberEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara Effects")
	float EffectDelayTimeEnd = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara Effects")
	bool isParticleOn = false;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime = 0;

	float ParticleDelayTimer = 0;

	
	
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{

	return (First + Second) / 2;
}
