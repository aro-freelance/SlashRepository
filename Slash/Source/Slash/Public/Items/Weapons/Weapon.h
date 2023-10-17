// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Items/Weapons/Projectile.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Weapon.generated.h"



class USoundBase;
class UBoxComponent;
class UAnimMontage;

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	//void Equip(USceneComponent* InParent, FName InSocketName);

	AWeapon();

	UFUNCTION(BlueprintCallable)
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

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool ActorIsSameType(AActor* OtherActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Name")
	FString WeaponName = "Default Weapon Name";


	/*
	* PROJECTILES
	*/

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TSubclassOf<AActor> Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	AProjectile* ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	FVector MuzzleOffset;

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = Projectile)
	void Fire();

	bool CanFire();
	bool CanDoDamage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	bool OverrideCanFire = false;

	FName GetRandomAttackMontageSectionName();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Properties")
	EWeaponType WeaponType = EWeaponType::EWT_OneHanded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Box")
	EWeaponCollisionState WeaponCollisionState = EWeaponCollisionState::EWS_CollisionOff;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	

	

	void BoxTrace(FHitResult& BoxHit);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	void MakeFieldAfterHit(FHitResult& BoxHit);

	FString BuildStateString();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	int32 PrecisionRange = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	int32 LowAccFloor = 75;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	int32 HighAccFloor = 95;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float PercentMagicDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	UDamageType* DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float CriticalMultiplier = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	float ProjectileSpeedMultiplier = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
	double FollowDistance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon FX")
	USoundBase* WeaponPickupSound;


	

private:

	UPROPERTY(VisibleAnywhere, Category = "Weapon Hit Box")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	//backup copy of animations in case they are overridden by character specific anims
	TArray<FName> OriginalAttackMontageSectionNames;

	//debugging
	bool CanAttack = true;
	bool CanCollide = true;
	

	

//Getters and Setters
public:
	
	//FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	

	FORCEINLINE double GetFollowDistance() const { return FollowDistance; }

	FORCEINLINE void SetItemState(EItemState NewItemState) { ItemState = NewItemState; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }


	FORCEINLINE EWeaponCollisionState GetWeaponCollisionState() const { return WeaponCollisionState; }

	FORCEINLINE float GetWeaponDamage() const { return Damage; }
	FORCEINLINE int32 GetPrecisionRange() const { return PrecisionRange; }
	FORCEINLINE int32 GetLowAccFloor() const { return LowAccFloor; }
	FORCEINLINE int32 GetHighAccFloor() const { return HighAccFloor; }
	FORCEINLINE float GetPercentMagicDamage() const { return PercentMagicDamage; }
	FORCEINLINE FString GetName() const { return WeaponName; }
	FORCEINLINE UDamageType* GetDamageType() const { return DamageType; }
	FORCEINLINE float GetCriticalMultiplier() const { return CriticalMultiplier; }
	FORCEINLINE float GetProjectileSpeedMultiplier() const { return ProjectileSpeedMultiplier; }


};
