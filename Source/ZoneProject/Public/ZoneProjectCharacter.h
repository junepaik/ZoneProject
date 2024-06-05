// Copyright Anton Romanov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZoneProjectTypes.h"
#include "GameFramework/Character.h"
#include "ZoneProjectCharacter.generated.h"

/**
 * Game Mode class
 */
UCLASS(Blueprintable)
class AZoneProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	/* Class constructor */
	AZoneProjectCharacter(const FObjectInitializer& ObjectInitializer);
	
	/* Set up replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Called before initializing components */
	virtual void PreInitializeComponents() override;

	/* Called after initializing components */
	virtual void PostInitializeComponents() override;

protected:

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/* Called when the game ends or when destroyed */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	/* Called every frame */
	virtual void Tick(float DeltaSeconds) override;

private:
	
	/* Camera boom positioning the camera above the character */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* Top-down camera */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* MainCamera;

protected:

	/* State properties */

	UPROPERTY(Category = "State", BlueprintReadOnly, EditDefaultsOnly)
	bool bIsAlive = true;

	UPROPERTY(Category = "State", BlueprintReadOnly, EditDefaultsOnly)
	bool bIsSprinting = false;

	/* Stats properties */
	
	UPROPERTY(Category = "Stats", BlueprintReadOnly, EditDefaultsOnly, Replicated)
	float MaxHealth = 100.f;

	UPROPERTY(Category = "Stats", BlueprintReadOnly, EditDefaultsOnly, Replicated)
	float Health = 100.f;

	/* Item properties */

	UPROPERTY(Category = "Items", BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<class AZoneProjectWeapon> DefaultWeaponClass;

	UPROPERTY(Category = "Items", BlueprintReadOnly, EditDefaultsOnly)
	FName WeaponSocketName = NAME_None;

	UPROPERTY(Category = "Items", BlueprintReadOnly, EditDefaultsOnly)
	TArray<FDropItemProbability> DropItemProbabilities;

	UPROPERTY(Category = "Items", BlueprintReadOnly, Replicated)
	AZoneProjectWeapon* Weapon;

	/* Timer handle for removing the character after death */
	FTimerHandle RemoveTimer;

	/* Called on the server upon receiving point damage */
	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	/* Randomly spawn a drop item on the character death based on the @DropItemProbabilities */
	void SpawnDropItem();

	/* Called from multicast on both the client and the server */
	void InternalOnDeath();
	
	/* Remove the character after death */
	UFUNCTION() virtual void RemoveCharacter();

public:
	
    /* Returns the camera boom sub-object */
    FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/* Return the main camera sub-object */
	FORCEINLINE UCameraComponent* GetMainCamera() const { return MainCamera; }

	/* Check whether the character is alive */
	bool IsAlive() const { return bIsAlive; }
	
	/* Check whether the character is sprinting */
	bool IsSprinting() const { return bIsSprinting; }

	/* Set the new health amount */
	UFUNCTION(Category = "Character", BlueprintCallable)
	void SetHealth(const float Value) { Health = FMath::Clamp(Value, 0.f, MaxHealth); }

	/* Increase the health amount by a specified value */
	UFUNCTION(Category = "Character", BlueprintCallable)
	void AddHealth(const float Value) { SetHealth(Health + Value); }

	/* Decrease the health amount by a specified value */
	UFUNCTION(Category = "Character", BlueprintCallable)
	void SubtractHealth(const float Value) { SetHealth(Health - Value); }

	/* Return the current health amount */
	float GetHealth() const { return Health; }

	/* Return the maximum health amount */
	float GetMaxHealth() const  { return MaxHealth; }

	/* Return the health amount interpolated between 0 and 1 */
	UFUNCTION(Category = "Character", BlueprintCallable)
	float GetHealthAlpha() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }

	/* Return the weapon */
	AZoneProjectWeapon* GetWeapon() const { return Weapon; }

	/* Return the character aim rotation */
	virtual FRotator GetBaseAimRotation() const override;

	/* Check if the character can sprint */
	UFUNCTION(Category = "Character", BlueprintCallable)
	virtual bool CanSprint() const;
	
	/* Start sprinting */
	UFUNCTION(Category = "Character", BlueprintCallable)
	void Sprint();

	/* Stop sprinting */
	UFUNCTION(Category = "Character", BlueprintCallable)
	void UnSprint();

	/* Start fire */
	UFUNCTION(Category = "Character", BlueprintCallable)
	void StartFire();

	/* Stop fire */
	UFUNCTION(Category = "Character", BlueprintCallable)
	void StopFire();

	/* Simulate fire on the client */
	UFUNCTION(Category = "Character", BlueprintCallable)
	void SimulateFire(const FRotator Rotation);

public:

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeath();

	UFUNCTION(Server, Reliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FRotator Rotation);
};
