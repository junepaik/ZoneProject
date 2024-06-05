// Copyright Anton Romanov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "ZoneProjectTypes.h"
#include "GameFramework/PlayerController.h"
#include "ZoneProjectController.generated.h"

/**
 * Player Controller class
 */
UCLASS(Blueprintable)
class ZONEPROJECT_API AZoneProjectController : public APlayerController
{
	GENERATED_BODY()

public:

	/* Class constructor */
	AZoneProjectController();

protected:

	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:

	/* Called every frame */
	virtual void Tick(float DeltaSeconds) override;

protected:

	/* Indicates whether the controller has successfully synced time with the server */
	bool bHasSyncedTime = false;

	/* Defines how frequently the time should be synced with the server */
	float TimeSyncFrequency = 5.f;

	/* Timer handle for sending time-sync requests to the server */
	FTimerHandle TimeSyncTimer;

	/* Defines if the last time-sync request was successful */
	bool bTimeSyncLastStatus = true;

	/* Tracks how many time-sync failures have happened in a row */
	int32 TimeSyncErrorCount = 0;
	int32 TimeSyncErrorCountToDisconnect = 5;

	/* Maximum round trip duration to consider the received time info valid */
	float RoundTripThreshold = 0.4f;

	/* History of the latest calculated round trip values. It can be used to calculate the averaged value */
	FValueHistory RoundTripHistory;

	/* Maximum number of entries in the round trip history */
	int32 RoundTripHistoryMaxCount = 2;

	/* Averaged round trip value in seconds (how long it takes to send a request to the server and receive a response) */
	float RoundTripAverage = 0.f;

	/* History of the latest calculated time delta values. It can be used to calculate the averaged value */
	FValueHistory TimeDeltaHistory;

	/* Maximum number of entries in the time delta history */
	int32 TimeDeltaHistoryMaxCount = 10;

	/* Averaged time delta value in seconds (how far the local world time is behind the server time) */
	float TimeDeltaAverage = 0.f;

protected:
	
	/* Default mapping context */
	UPROPERTY(Category = "Input", BlueprintReadOnly, EditDefaultsOnly)
	class UInputMappingContext* DefaultMappingContext = nullptr;

	/* Input bindings */
	
	UPROPERTY(Category = "Input", BlueprintReadOnly, EditDefaultsOnly)
	class UInputAction* MoveAction = nullptr;

	UPROPERTY(Category = "Input", BlueprintReadOnly, EditDefaultsOnly)
	class UInputAction* FireAction = nullptr;

	UPROPERTY(Category = "Input", BlueprintReadOnly, EditDefaultsOnly)
	class UInputAction* SprintAction = nullptr;

	/* Input states */

	UPROPERTY(Category = "Input", BlueprintReadOnly)
	bool bFirePressed = false;

	UPROPERTY(Category = "Input", BlueprintReadOnly)
	bool bSprintPressed = false;

	/* Character currently possessed by this controller */
	UPROPERTY(Category = "General", BlueprintReadOnly)
	class AZoneProjectCharacter* ControlledCharacter = nullptr;

protected:
	
	/* Set up input bindings */
	virtual void SetupInputComponent() override;

	/* Called when this controller is asked to possess a pawn (server) */
	virtual void OnPossess(APawn* InPawn) override;

	/* Called when this controller is asked to un-possess a pawn (server) */
	virtual void OnUnPossess() override;

	/* Called after possessing a new pawn (standalone, listen server, client) */
	virtual void AcknowledgePossession(APawn* InPawn) override;

	/* Trigger the time sync request to further calculate the round trip and the time delta */
	void SyncTime();

	/* Input handlers */
	
	void OnMoveTriggered(const FInputActionInstance& InputAction);
	void OnFireStarted(const FInputActionInstance& InputAction);
	void OnFireCompleted(const FInputActionInstance& InputAction);
	void OnSprintStarted(const FInputActionInstance& InputAction);
	void OnSprintCompleted(const FInputActionInstance& InputAction);

public:

	/* Return the averaged round trip value in seconds or milliseconds */
	UFUNCTION(Category = "Network", BlueprintCallable)
	float GetRoundTrip(bool bMilliseconds = false) const { return RoundTripAverage * (bMilliseconds ? 1000 : 1); }

	/* Return the averaged time delta value in seconds */
	UFUNCTION(Category = "Network", BlueprintCallable)
	float GetTimeDelta() const { return TimeDeltaAverage; }

	/* Return the actual local time in seconds */
	UFUNCTION(Category = "Network", BlueprintCallable)
	float GetLocalTime() const { return GetWorld()->GetTimeSeconds(); }

	/* Return the simulated server time in seconds */
	UFUNCTION(Category = "Network", BlueprintCallable)
	float GetServerTime() const { return GetLocalTime() + TimeDeltaAverage; }

protected:

	/* Request the current time from the server */
	UFUNCTION(Server, Reliable)
	void ServerRequestTime(const float ClientTime);

	/* Report the current time to the client */
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(const float ClientTime, float ServerTime);
};
