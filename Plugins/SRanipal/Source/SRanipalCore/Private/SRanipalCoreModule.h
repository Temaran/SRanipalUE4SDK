/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#pragma once

#include "ISRanipalCore.h"

#include "CoreMinimal.h"
#include "IInputDevice.h"
#include "DisplayDebugHelpers.h"

class FSRanipalCoreModule : public ISRanipalCore
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual TSharedPtr<class IEyeTracker, ESPMode::ThreadSafe> CreateEyeTracker() override;
	virtual bool IsEyeTrackerConnected() const override;

protected:
	virtual TSharedPtr<ISRanipalEyeTracker, ESPMode::ThreadSafe> GetEyeTrackerInternal() override;

private:
	TSharedPtr<ISRanipalEyeTracker, ESPMode::ThreadSafe> EyeTracker;
	void* SRanipalDllHandle;
};
