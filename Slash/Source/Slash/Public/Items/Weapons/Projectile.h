// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/SlashCharacter.h"
#include "Projectile.generated.h"



class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class SLASH_API AProjectile : public AActor
{
	GENERATED_BODY()

public:

	AProjectile();

	void SetCharacterWhoFired(ASlashCharacter* Character) { CharacterWhoFiredThis = Character; }


	//TODO: this is not currently detecting hits. Fix this.
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UStaticMesh* DefaultMesh;

private:

	ASlashCharacter* CharacterWhoFiredThis;


};

