// Copyright Anton Romanov. All Rights Reserved.

#include "ZoneProjectCharacterMovement.h"
#include "ZoneProjectCharacter.h"

UZoneProjectCharacterMovement::UZoneProjectCharacterMovement(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SprintMaxWalkSpeed = 600.f;
	SprintMaxAcceleration = 1000.f;

	MaxWalkSpeed = 300.f;
	MaxAcceleration = 1000.f;

	BrakingFrictionFactor = 1.f;
	BrakingDecelerationWalking = 1000.f;
}

float UZoneProjectCharacterMovement::GetMaxSpeed() const
{
	if (const AZoneProjectCharacter* CharacterCasted = Cast<AZoneProjectCharacter>(CharacterOwner))
	{
		if (IsWalking() && CharacterCasted->IsSprinting())
		{
			return SprintMaxWalkSpeed;
		}
	}

	return Super::GetMaxSpeed();
}

float UZoneProjectCharacterMovement::GetMaxAcceleration() const
{
	if (const AZoneProjectCharacter* CharacterCasted = Cast<AZoneProjectCharacter>(CharacterOwner))
	{
		if (IsWalking() && CharacterCasted->IsSprinting())
		{
			return SprintMaxAcceleration;
		}
	}
	
	return Super::GetMaxAcceleration();
}

void UZoneProjectCharacterMovement::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	if (!CharacterOwner) return;

	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	
	// Simulate actions on the server

	if (CharacterOwner->HasAuthority())
	{
		if (AZoneProjectCharacter* CharacterCasted = Cast<AZoneProjectCharacter>(CharacterOwner))
		{
			if (bWantsToSprint) CharacterCasted->Sprint(); else CharacterCasted->UnSprint();
		}
	}
}

FNetworkPredictionData_Client* UZoneProjectCharacterMovement::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UZoneProjectCharacterMovement* MutableThis = const_cast<UZoneProjectCharacterMovement*>(this);
		MutableThis->ClientPredictionData = new FExtNetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}

void FExtSavedMove_Character::Clear()
{
	Super::Clear();

	bWantsToSprint  = false;
}

void FExtSavedMove_Character::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
	
	if (const UZoneProjectCharacterMovement* CharacterMovement = Cast<UZoneProjectCharacterMovement>(Character->GetCharacterMovement()))
	{
		bWantsToSprint  = CharacterMovement->bWantsToSprint;
	}
}

void FExtSavedMove_Character::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);
}

bool FExtSavedMove_Character::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (bWantsToSprint != ((FExtSavedMove_Character*)&NewMove)->bWantsToSprint) return false;

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

uint8 FExtSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bWantsToSprint) Result |= FLAG_Custom_0;

	return Result;
}

FExtNetworkPredictionData_Client_Character::FExtNetworkPredictionData_Client_Character
	(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement) {}

FSavedMovePtr FExtNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FExtSavedMove_Character());
}
