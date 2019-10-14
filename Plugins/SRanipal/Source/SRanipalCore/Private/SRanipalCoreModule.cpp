/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#include "SRanipalCoreModule.h"
#include "SRanipalEyeTracker.h"

#include "GameFramework/HUD.h"
#include "Misc/Paths.h"

IMPLEMENT_MODULE(FSRanipalCoreModule, SRanipalCore)

void FSRanipalCoreModule::StartupModule()
{
	EyeTracker.Reset();

#if SRANIPAL_EYETRACKING_ACTIVE
	FString RelativeSRanipalDllPath = FString(TEXT(SRANIPAL_RELATIVE_DLL_PATH));
	FString RelativeSRanipalBinariesPath = FString(TEXT(SRANIPAL_RELATIVE_BINARY_PATH));

#if SRANIPAL_COMPILE_AS_ENGINE_PLUGIN
	FString FullSRanipalDllPath = FPaths::Combine(FPaths::EngineDir(), RelativeSRanipalDllPath);
	FString FullSRanipalBinariesPath = FPaths::Combine(FPaths::EngineDir(), RelativeSRanipalBinariesPath);
#else
	FString FullSRanipalDllPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectPluginsDir(), RelativeSRanipalDllPath));
	FString FullSRanipalBinariesPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectPluginsDir(), RelativeSRanipalBinariesPath));
#endif // SRANIPAL_COMPILE_AS_ENGINE_PLUGIN

	FPlatformProcess::AddDllDirectory(*FullSRanipalBinariesPath);
	SRanipalDllHandle = FPlatformProcess::GetDllHandle(*FullSRanipalDllPath);

	if (SRanipalDllHandle != nullptr)
	{
		FSRanipalEyeTracker* NewEyeTracker = new FSRanipalEyeTracker();
		if (NewEyeTracker != nullptr && NewEyeTracker->IsEyeTrackerConnected())
		{
			IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);
			EyeTracker = TSharedPtr<ISRanipalEyeTracker, ESPMode::ThreadSafe>(NewEyeTracker);
		}
	}
#endif // SRANIPAL_EYETRACKING_ACTIVE
}

void FSRanipalCoreModule::ShutdownModule()
{
	if (EyeTracker.IsValid())
	{
		IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
		EyeTracker.Reset();
	}
}

TSharedPtr<IEyeTracker, ESPMode::ThreadSafe> FSRanipalCoreModule::CreateEyeTracker()
{
	return StaticCastSharedPtr<IEyeTracker, ISRanipalEyeTracker, ESPMode::ThreadSafe>(EyeTracker);
}

bool FSRanipalCoreModule::IsEyeTrackerConnected() const
{
	return EyeTracker.IsValid();
}

TSharedPtr<ISRanipalEyeTracker, ESPMode::ThreadSafe> FSRanipalCoreModule::GetEyeTrackerInternal()
{
	return EyeTracker;
}
