/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#pragma once

#include "SRanipalTypes.h"

#include "CoreMinimal.h"
#include "IEyeTracker.h"

class ISRanipalEyeTracker : public IEyeTracker
{
	/************************************************************************/
	/* Common Data                                                          */
	/************************************************************************/
public:
	/**
	  * Will return gaze data for a combination of both eyes.
	  * For mono trackers this might mean sending the data from the active eye.
	  * For a more advanced system, it might be some combination of both eyes.
	  * It contains all kinds of data you might need for low level interaction.
	  *
	  * @returns	Combined gaze data.
	  */
	virtual const FSRanipalGazeData& GetCombinedGazeData() const = 0;

	/**
	  * Will return the gaze data for the left eye.
	  * It contains all kinds of data you might need for low level interaction.
	  *
	  * @returns	Gaze data for the right eye.
	  */
	virtual const FSRanipalGazeData& GetLeftGazeData() const = 0;

	/**
	  * Will return the gaze data for the right eye.
	  * It contains all kinds of data you might need for low level interaction.
	  *
	  * @returns	Gaze data for the left eye.
	  */
	virtual const FSRanipalGazeData& GetRightGazeData() const = 0;
};
