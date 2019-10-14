/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "ISRanipalEyetracker.h"

#include "IEyeTrackerModule.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/*
 * To get this to work, you must:
 * 1. Put this plugin in your project
 * 2. Put the SRanipal SDK includes and binaries in the right spots under the ThirdParty folder of this plugin
 * 3. Patch the header SRanipal_EyeDataType.h header file since it doesn't compile
 * 4. If you want to try the social scene, you need to patch the Sample/Common/Blueprints/BP_MirrorRobot blueprint to use SRanipal data instead of Tobii gaze data.
 */

class ISRanipalCore : public IEyeTrackerModule
{
public:
	/**
	  * Singleton-like access to this module's interface.  This is just for convenience!
	  * Beware of calling this during the shutdown phase, though. Your module might have been
	  * unloaded already.
	  *
	  * @return Returns singleton instance, loading the module on demand if needed
	  */
	static inline ISRanipalCore& Get()
	{
		return FModuleManager::LoadModuleChecked<ISRanipalCore>("SRanipalCore");
	}

	/**
	  * Checks to see if this module is loaded and ready.  It is only valid to call Get() if 
	  * IsAvailable() returns true.
	  *
	  * @return True if the module is loaded and ready to use
	  */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("SRanipalCore");
	}

	virtual FString GetModuleKeyName() const override
	{
		return FString("SRanipalCore");
	}

	static TSharedPtr<ISRanipalEyeTracker, ESPMode::ThreadSafe> GetEyeTracker()
	{
		return IsAvailable() ? Get().GetEyeTrackerInternal() : TSharedPtr<ISRanipalEyeTracker, ESPMode::ThreadSafe>(nullptr);
	}

protected:
	virtual TSharedPtr<ISRanipalEyeTracker, ESPMode::ThreadSafe> GetEyeTrackerInternal() = 0;
};
