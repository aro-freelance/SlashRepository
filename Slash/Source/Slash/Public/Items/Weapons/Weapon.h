// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Weapon.generated.h"


class USoundBase;
class UBoxComponent;

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	//void Equip(USceneComponent* InParent, FName InSocketName);

	AWeapon();

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	void DetachMeshFromSocket();
	
	void PlayWeaponPickupSound();

	UPROPERTY(EditAnywhere, Category = "Rotation")
	FRotator StartingRotation = FRotator(0 , 0 , 0);

	//this adjusts the size of the boxtracesingle. for many weapons 25.f is good for some, with difficult to hit animations larger is better.
	UPROPERTY(EditAnywhere, Category = "Collision")
	FVector HalfSize = FVector(25.f, 25.f, 25.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TArray<AActor*> HitActorsToIgnore;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EWeaponSize WeaponSize = EWeaponSize::EWS_OneHanded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Box")
	EWeaponCollisionState WeaponCollisionState = EWeaponCollisionState::EWS_CollisionOff;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

	bool CanDoDamage();

private:

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* WeaponPickupSound;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;


	//debugging
	bool CanAttack = true;
	bool CanCollide = true;
	
	

//Getters and Setters
public:
	
	//FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	
	FORCEINLINE EWeaponSize GetWeaponSize() const { return WeaponSize; }

	FORCEINLINE void SetItemState(EItemState NewItemState) { ItemState = NewItemState; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }


	FORCEINLINE EWeaponCollisionState GetWeaponCollisionState() const { return WeaponCollisionState; }
	
};
