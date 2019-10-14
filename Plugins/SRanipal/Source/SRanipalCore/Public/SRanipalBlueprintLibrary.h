/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "SRanipalTypes.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/WidgetComponent.h"

#include "SRanipalBlueprintLibrary.generated.h"

/**
  * Simplified interface for blueprint use. Only exposes the features most likely to be consumed from BP.
  */
UCLASS()
class SRANIPALCORE_API USRanipalBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/************************************************************************/
	/* Common functions                                                     */
	/************************************************************************/
public:
	/**
	  * This is the main method to get basic eye tracking data. You should not be using this though for most things. Use the focus system instead if you can.
	  */
	UFUNCTION(BlueprintPure, Category = "SRanipal Eyetracking")
	static FSRanipalGazeData GetSRanipalCombinedGazeData();
	UFUNCTION(BlueprintPure, Category = "SRanipal Eyetracking")
	static FSRanipalGazeData GetSRanipalLeftGazeData();
	UFUNCTION(BlueprintPure, Category = "SRanipal Eyetracking")
	static FSRanipalGazeData GetSRanipalRightGazeData();
};
