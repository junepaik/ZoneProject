// Copyright Anton Romanov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ZoneProjectTypes.generated.h"

/**
 * Custom types collection
 */

USTRUCT(BlueprintType)
struct ZONEPROJECT_API FValueHistory
{
	GENERATED_USTRUCT_BODY()

protected:

	/* List of values */
	TArray<float> Values;

	/* Maximum number of values in the list */
	int32 MaxCount = 2;

public:

	/* Constructor */
	explicit FValueHistory(const int32 InMaxCount = 2)
	{ 
		Init(InMaxCount);
	}

	/* Initialize the history */
	void Init(const int32 InMaxCount = 2)
	{
		Values.Empty();
		MaxCount = FMath::Clamp(InMaxCount, 2, 255);
	}

	/* Clear the list */
	void Clear()
	{
		Values.Empty();
	}

	/* Add a value to the beginning of the list */
	void Insert(const float Value)
	{
		Values.Insert(Value, 0);
		while (Values.Num() > MaxCount) Values.RemoveAt(MaxCount);
	}

	/* Add a value to the end of the list */
	void Add(const float Value)
	{
		if (Values.Num() < MaxCount) Values.Add(Value);
	}

	/* Return the list of values */
	TArray<float> GetValues() const
	{
		return Values;
	}

	/* Return the maximum count of values in the list */
	int32 GetMaxCount() const
	{
		return MaxCount;
	}

	/* Find the minimum value in the list */
	float Min() const
	{
		if (Values.Num() == 0) return 0.f;
		return FMath::Min(Values);
	}

	/* Find the maximum value in the list */
	float Max() const
	{
		if (Values.Num() == 0) return 0.f;
		return FMath::Max(Values);
	}

	/* Calculate the average value of the list */
	float Average() const
	{
		if (Values.Num() == 0) return 0.f;

		float Sum = 0.f;
		for (auto& Value : Values) Sum += Value;
		return Sum / Values.Num();
	}
};

USTRUCT(BlueprintType)
struct ZONEPROJECT_API FDropItemProbability
{
	GENERATED_USTRUCT_BODY()
 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class AZoneProjectDropItem> ItemClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ClampMin = "0", UIMin = "0", ClampMax = "1", UIMax = "1"))
	float Value = 1.f;
};
