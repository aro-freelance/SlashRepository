// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/SlashCharacter.h"
#include "Projectile.generated.h"


class UProjectileMovementComponent;

UCLASS()
class SLASH_API AProjectile : public AActor
{
	GENERATED_BODY()

public:

	AProjectile();
	virtual void Tick(float DeltaTime) override;

	void SetCharacterWhoFired(ASlashCharacter* Character) { CharacterWhoFiredThis = Character; }

	void SetWeaponThatFired(AWeapon* Weapon) { WeaponThatFiredThis = Weapon; }


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Impulse)
	bool HasInitialImpulse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Impulse)
	float OffsetAmount = 2;

protected:
	virtual void BeginPlay() override;

	void InitializeShooterInfo();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UStaticMesh* DefaultMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Owner)
	ASlashCharacter* CharacterWhoFiredThis;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Owner)
	AWeapon* WeaponThatFiredThis;


private:

	


};

