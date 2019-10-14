/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

#include "SRanipalTypes.generated.h"

/**
  * Structure that contains gaze information from one eye in both screen and world space.
  */
USTRUCT(BlueprintType)
struct FSRanipalGazeData
{
	GENERATED_USTRUCT_BODY()

public:
	//Origin of the eye's gaze ray in Unreal world space.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Space Gaze Data")
	FVector WorldGazeOrigin;
	//Forward direction of the eye's gaze ray this frame in Unreal world space.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "World Space Gaze Data")
	FVector WorldGazeDirection;

	//Time when the gaze point was created.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Data")
	FDateTime TimeStamp;
	//This is how open the eye is. 0 means closed, 1 means open.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Data")
	float EyeOpenness;
	//If this is true, all other properties related to the eye should be safe to use. This might be false because the tracking system is a mono tracker, or if the data is bad for this frame.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Data")
	bool bIsGazeDataValid;

	FSRanipalGazeData()
		: bIsGazeDataValid(false)
	{
	}
};
