// Copyright Anton Romanov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZoneProjectTypes.h"
#include "GameFramework/Actor.h"
#include "ZoneProjectWeapon.generated.h"

/**
 * Weapon class
 */
UCLASS(Blueprintable)
class ZONEPROJECT_API AZoneProjectWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	/* Class constructor */
	AZoneProjectWeapon();

	/* Set up property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Called before initializing components */
	virtual void PreInitializeComponents() override;

	/* Called after initializing components */
	virtual void PostInitializeComponents() override;

protected:

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;
	
private:
	
	UPROPERTY(Category = "Components", BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* Mesh;

protected:

	/* Owning character reference */
	UPROPERTY(Category = "General", BlueprintReadOnly)
	class AZoneProjectCharacter* Character = nullptr;

	/* rate of spawning projectiles */
	UPROPERTY(Category = "Stats", BlueprintReadOnly)
	float FireRate = 0.1f;
	
public:

	/* Called after the instigator is replicated */
	virtual void OnRep_Instigator() override;
	
	/* Return the mesh component */
	USkeletalMeshComponent* GetMesh() const { return Mesh; }

	/* Return the instigator character actor */
	AZoneProjectCharacter* GetCharacter() const { return Character; }
	
	/* Blueprint implementable events */

	UFUNCTION(Category = "Weapon", BlueprintImplementableEvent, BlueprintCallable)
	void StartFire();

	UFUNCTION(Category = "Weapon", BlueprintImplementableEvent, BlueprintCallable)
	void ContinueFire();

	UFUNCTION(Category = "Weapon", BlueprintImplementableEvent, BlueprintCallable)
	void StopFire();

	UFUNCTION(Category = "Weapon", BlueprintImplementableEvent, BlueprintCallable)
	void SimulateFire(const FRotator Rotation);

	UFUNCTION(Category = "Weapon", BlueprintCallable)
	void ReplicateFire(const FRotator Rotation);
};
