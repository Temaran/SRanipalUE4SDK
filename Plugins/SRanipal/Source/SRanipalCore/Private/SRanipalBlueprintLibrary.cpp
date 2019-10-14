/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#include "SRanipalBlueprintLibrary.h"
#include "SRanipalCoreModule.h"

#include "Engine/Engine.h"

USRanipalBlueprintLibrary::USRanipalBlueprintLibrary(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

FSRanipalGazeData USRanipalBlueprintLibrary::GetSRanipalCombinedGazeData()
{
	static FSRanipalGazeData DummyData;
	return FSRanipalCoreModule::IsAvailable() ? FSRanipalCoreModule::GetEyeTracker()->GetCombinedGazeData() : DummyData;
}

FSRanipalGazeData USRanipalBlueprintLibrary::GetSRanipalLeftGazeData()
{
	static FSRanipalGazeData DummyData;
	return FSRanipalCoreModule::IsAvailable() ? FSRanipalCoreModule::GetEyeTracker()->GetLeftGazeData() : DummyData;
}

FSRanipalGazeData USRanipalBlueprintLibrary::GetSRanipalRightGazeData()
{
	static FSRanipalGazeData DummyData;
	return FSRanipalCoreModule::IsAvailable() ? FSRanipalCoreModule::GetEyeTracker()->GetRightGazeData() : DummyData;
}
