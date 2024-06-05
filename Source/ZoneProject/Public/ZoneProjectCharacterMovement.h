// Copyright Anton Romanov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZoneProjectTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZoneProjectCharacterMovement.generated.h"

/**
 * Character Movement Component class
 */
UCLASS()
class ZONEPROJECT_API UZoneProjectCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	/* Class constructor */
	UZoneProjectCharacterMovement(const FObjectInitializer& ObjectInitializer);

	/* Indicates whether the character is capable of sprinting */
	UPROPERTY(Category = "Character Movement (Sprint Settings)", BlueprintReadWrite, EditAnywhere)
	bool bCanSprint = true;

	/* Maximum walk speed when sprinting */
	UPROPERTY(Category = "Character Movement (Sprint Settings)", BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0", UIMin = "0", ForceUnits="cm/s"))
	float SprintMaxWalkSpeed;

	/* Maximum acceleration when sprinting */
	UPROPERTY(Category = "Character Movement (Sprint Settings)", BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0", UIMin = "0"))
	float SprintMaxAcceleration;

	/* Sprint movement flag */
	uint8 bWantsToSprint : 1;

	/* Return maximum speed for the current state. */
	virtual float GetMaxSpeed() const override;

	/* Return maximum acceleration for the current state. */
	virtual float GetMaxAcceleration() const override;

	/* Unpack compressed flags from a saved move and set the movement state accordingly */
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	/* Get network prediction data for a client game */
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
};

/**
 * Override Character Saved Move class
 */
class FExtSavedMove_Character : public FSavedMove_Character
{
public:

	typedef FSavedMove_Character Super;

	/* Sprint movement flag */
	uint8 bWantsToSprint : 1;

	/* Reset saved properties to the initial state */
	virtual void Clear() override;

	/* Set up this saved move (when initially created) to make a predictive correction */
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
		FNetworkPredictionData_Client_Character& ClientData) override;

	/* Set properties on the character movement component before making a predictive correction */
	virtual void PrepMoveFor(ACharacter* Character) override;

	/* Check whether this move can be combined with the new move for replication without changing any behavior */
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	/* Get a byte containing encoded special movement information (jumping, crouching, sprinting) */
	virtual uint8 GetCompressedFlags() const override;
};

/**
 * Override Client Network Prediction Data class
 */
class FExtNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
public:
	
	FExtNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	/* Allocate new copy of a saved move */
	virtual FSavedMovePtr AllocateNewMove() override;
};
