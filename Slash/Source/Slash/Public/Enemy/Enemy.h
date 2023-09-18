// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, ASlashCharacter* DamageDealer, float Damage = 20.f, int32 PrecisionRange = 15, int32 LowAccFloor = 75, int32 HighAccFloor = 95, float PercentMagicDamage = 0.0f, const FString& WeaponName = "default") override;

	void DirectionalHitReact(const FVector& ImpactPoint);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FString GetName();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
	FRotator HitParticleRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Effects")
	FVector HitParticleScale = FVector(1.f, 1.f, 1.f);

	/*
	* Play montage functions
	*/

	void PlayHitReactMontage(const FName& SectionName);

	float CalculatePhysicalDamage(float DamageAmount);
	float CalculateMagicalDamage(float DamageAmount);

	void DamageTakenUpdateHUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Name")
	FString EnemyName = "Default Enemy Name";


private:

	float CompareSTRVIT(float AttackerSTR, float DefenderVIT);
	int32 CompareDEXAGI(float AttackerDEX, float DefenderAGI);
	float CompareINTMND(float AttackerINT, float DefenderMND);
	int32 CompareCHR(float AttackerCHR, float DefenderCHR);


	//the most recent character to damage this Enemy
	UPROPERTY(VisibleAnywhere, Category = "Character Who Damaged Enemy Properties")
	ASlashCharacter* CharacterWhoDamagedEnemy;
	UPROPERTY(VisibleAnywhere, Category = "Character Who Damaged Enemy Properties")
	int32 DamagerPrecisionRange;
	UPROPERTY(VisibleAnywhere, Category = "Character Who Damaged Enemy Properties")
	int32 DamagerLowAccFloor;
	UPROPERTY(VisibleAnywhere, Category = "Character Who Damaged Enemy Properties")
	int32 DamagerHighAccFloor;
	UPROPERTY(VisibleAnywhere, Category = "Character Who Damaged Enemy Properties")
	float DamagerPercentMagicDamage;
	UPROPERTY(VisibleAnywhere, Category = "Character Who Damaged Enemy Properties")
	FString DamagerWeaponName;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	/*
	* Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UParticleSystem* HitParticles;

};
