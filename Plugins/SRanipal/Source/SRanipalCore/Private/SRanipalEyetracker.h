/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#pragma once

#if SRANIPAL_EYETRACKING_ACTIVE

#include "CoreMinimal.h"
#include "ISRanipalEyetracker.h"

#include "Containers/Ticker.h"
#include "GameFramework/PlayerController.h"
#include "Slate/SceneViewport.h"

class FSRanipalEyeTracker : public ISRanipalEyeTracker, public FTickerObjectBase
{
public:
	FSRanipalEyeTracker();
	virtual ~FSRanipalEyeTracker();
	virtual bool Tick(float DeltaTime) override;
	void Shutdown();
	bool IsEyeTrackerConnected() { return bSRanipalInitialized; }
	
	/************************************************************************/
	/* ISRanipalEyeTracker                                                  */
	/************************************************************************/
public:
	virtual const FSRanipalGazeData& GetCombinedGazeData() const override;
	virtual const FSRanipalGazeData& GetLeftGazeData() const override;
	virtual const FSRanipalGazeData& GetRightGazeData() const override;

	/************************************************************************/
	/* IEyeTracker                                                          */
	/************************************************************************/
public:
	virtual void SetEyeTrackedPlayer(APlayerController* PlayerController) override;
	virtual bool IsStereoGazeDataAvailable() const override { return true; }
	virtual bool GetEyeTrackerGazeData(FEyeTrackerGazeData& OutGazeData) const override;
	virtual bool GetEyeTrackerStereoGazeData(FEyeTrackerStereoGazeData& OutGazeData) const override;
	virtual EEyeTrackerStatus GetEyeTrackerStatus() const override;
	
private:
	void ResetData();

	TWeakObjectPtr<APlayerController> ActivePlayerController;
	bool bSRanipalInitialized;

	FSRanipalGazeData LeftGazeData;
	FSRanipalGazeData RightGazeData;
	FSRanipalGazeData CombinedGazeData;
	FEyeTrackerGazeData GazeData;
	FEyeTrackerStereoGazeData StereoGazeData;
	EEyeTrackerStatus GazeTrackerStatus;
};

#endif // SRANIPAL_EYETRACKING_ACTIVE
