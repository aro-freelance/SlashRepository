// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy/EnemyTypes.h"
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

	virtual void GetHit_Implementation(const FVector& ImpactPoint, ASlashCharacter* DamageDealer, AWeapon* Weapon);
	

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Death();

	FString GetName();

	double HideDistance();

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
	void PlayDeathMontage(const FName& SectionName);

	float CalculatePhysicalDamage(float DamageAmount);
	float CalculateMagicalDamage(float DamageAmount);


	void UpdateCombatHUD();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Name")
	FString EnemyName = "Default Enemy Name";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	double CombatRadius = 2500.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsInCombat = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsRegening = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsReturning = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsChangingDirection = false;

	bool CheckCritical(const FVector& ImpactPoint);

	void EndCombat();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float MaxHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float RegenPercent;
	UFUNCTION(BlueprintCallable)
	void Recover();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	//the most recent character to damage this Enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	ASlashCharacter* CharacterWhoDamagedEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	AWeapon* WeaponThatDamagedEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	FVector LastHitImpactPoint = FVector();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	FName LastHitDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damaged By Properties")
	float LastDamageAmount;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regen")
	float RegenTickLength = 150.0f;

private:

	float CompareSTRVIT(float AttackerSTR, float DefenderVIT);
	int32 CompareDEXAGI(float AttackerDEX, float DefenderAGI);
	float CompareINTMND(float AttackerINT, float DefenderMND);
	int32 CompareCHR(float AttackerCHR, float DefenderCHR);


	float RegenTickTimer = 0.0f;
	

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UParticleSystem* HitParticles;

	

	/*
	* Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	FName CalculateHitReactSectionName(const FVector& ImpactPoint);
	FName CalculateDeathMontageSectionName();
	
	
};
