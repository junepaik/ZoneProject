// Copyright Anton Romanov. All Rights Reserved.

#include "ZoneProjectController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ZoneProjectCharacter.h"
#include "ZoneProjectWeapon.h"
#include "Kismet/KismetMathLibrary.h"

AZoneProjectController::AZoneProjectController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AZoneProjectController::BeginPlay()
{
	Super::BeginPlay();

	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.SetTimer(TimeSyncTimer, this, &AZoneProjectController::SyncTime, TimeSyncFrequency, true, 0.f);
}

void AZoneProjectController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalController() && ControlledCharacter)
	{
		const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	
		if (FHitResult HitResult; GetHitResultUnderCursorByChannel(TraceChannel, true, HitResult))
		{
			FVector EyePoint = ControlledCharacter->GetActorLocation();
			EyePoint.Z += ControlledCharacter->BaseEyeHeight;

			// Find look at the cursor point
			FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(EyePoint, HitResult.ImpactPoint);
			
			// Limit the character pitch
			NewRotation.Pitch = FMath::Clamp(static_cast<float>(NewRotation.Pitch), -60.f, 60.f);

			SetControlRotation(NewRotation);
			
		}
	}
}

void AZoneProjectController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add input mapping context
	
	if (DefaultMappingContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction,   ETriggerEvent::Triggered, this, &AZoneProjectController::OnMoveTriggered);
		EnhancedInputComponent->BindAction(FireAction,   ETriggerEvent::Started,   this, &AZoneProjectController::OnFireStarted);
		EnhancedInputComponent->BindAction(FireAction,   ETriggerEvent::Completed, this, &AZoneProjectController::OnFireCompleted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started,   this, &AZoneProjectController::OnSprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AZoneProjectController::OnSprintCompleted);
	}
}

void AZoneProjectController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AZoneProjectCharacter* CharacterCasted = Cast<AZoneProjectCharacter>(InPawn))
	{
		ControlledCharacter = CharacterCasted;
	}
}

void AZoneProjectController::OnUnPossess()
{
	Super::OnUnPossess();

	ControlledCharacter = nullptr;
}

void AZoneProjectController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);

	if (AZoneProjectCharacter* CharacterCasted = Cast<AZoneProjectCharacter>(InPawn))
	{
		ControlledCharacter = CharacterCasted;
	}
}

void AZoneProjectController::SyncTime()
{
	ServerRequestTime(GetLocalTime());
}

void AZoneProjectController::OnMoveTriggered(const FInputActionInstance& InputAction)
{
	if (ControlledCharacter)
	{
		const FVector2d ActionValue = InputAction.GetValue().Get<FVector2d>();
		ControlledCharacter->AddMovementInput(UKismetMathLibrary::Conv_Vector2DToVector(ActionValue));
	}
}

void AZoneProjectController::OnFireStarted(const FInputActionInstance& InputAction)
{
	bFirePressed = true;

	if (ControlledCharacter)
	{
		if (AZoneProjectWeapon* Weapon = ControlledCharacter->GetWeapon())
		{
			Weapon->StartFire();
		}
	}
}

void AZoneProjectController::OnFireCompleted(const FInputActionInstance& InputAction)
{
	bFirePressed = false;

	if (ControlledCharacter)
	{
		if (AZoneProjectWeapon* Weapon = ControlledCharacter->GetWeapon())
		{
			Weapon->StopFire();
		}
	}
}

void AZoneProjectController::OnSprintStarted(const FInputActionInstance& InputAction)
{
	bSprintPressed = true;

	if (ControlledCharacter)
	{
		ControlledCharacter->Sprint();
	}
}

void AZoneProjectController::OnSprintCompleted(const FInputActionInstance& InputAction)
{
	bSprintPressed = false;

	if (ControlledCharacter)
	{
		ControlledCharacter->UnSprint();
	}
}

void AZoneProjectController::ServerRequestTime_Implementation(const float ClientTime)
{
	const float ServerTime = GetLocalTime();
	ClientReportServerTime(ClientTime, ServerTime);
}

void AZoneProjectController::ClientReportServerTime_Implementation(const float ClientTime, float ServerTime)
{
	const float WorldTime = GetLocalTime();
	const float RoundTrip = WorldTime - ClientTime;
	const float Latency = RoundTrip * 0.5f;

	RoundTripHistory.Insert(RoundTrip);
	RoundTripAverage = RoundTripHistory.Average();

	//UE_LOG(LogTemp, Warning, TEXT("SyncTime Result: Round Trip: %f; Round Trip Average: %f"), RoundTrip, RoundTripAverage);

	if (RoundTrip <= RoundTripThreshold)
	{
		const float TimeDelta = (ServerTime - WorldTime) + Latency;

		TimeDeltaHistory.Insert(TimeDelta);
		TimeDeltaAverage = TimeDeltaHistory.Average();

		bTimeSyncLastStatus = bHasSyncedTime = true;
		TimeSyncErrorCount = 0;

		//UE_LOG(LogTemp, Warning, TEXT("SyncTime Result: Time Delta: %f; Time Delta Average: %f"), TimeDelta, TimeDeltaAverage);
	}
	else
	{
		bTimeSyncLastStatus = false;
		TimeSyncErrorCount++;

		//UE_LOG(LogTemp, Warning, TEXT("SyncTime Result: Round trip has exceeded the threshold: %f"), RoundTripThreshold);
	}
}
