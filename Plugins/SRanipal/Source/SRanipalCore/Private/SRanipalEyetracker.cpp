/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

#include "SRanipalEyetracker.h"

#include "DrawDebugHelpers.h"
#include "IXRTrackingSystem.h"
#include "Engine/Engine.h"
#include "Engine/Canvas.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SWindow.h"
#include "Misc/App.h"

/************************************************************************/
/* Configuration CVars                                                  */
/************************************************************************/
static TAutoConsoleVariable<int32> CVarSRanipalEnableEyetracking(TEXT("SRanipal.EnableEyetracking"), 1, TEXT("0 - Eyetracking is disabled. 1 - Eyetracking is enabled."));

static TAutoConsoleVariable<int32> CVarSRanipalEnableEyetrackingDebug(TEXT("SRanipal.debug"), 0, TEXT("0 - Eyetracking debug visualizations are disabled. 1 - Eyetracking debug visualizations are enabled."));
static TAutoConsoleVariable<int32> CVarSRanipalEnableGazePointDebug(TEXT("SRanipal.debug.EnableGazePointDebug"), 1, TEXT("0 - Gaze point debug visualizations are disabled. 1 - Gaze point debug visualizations are enabled."));

#if SRANIPAL_EYETRACKING_ACTIVE

#include "SRanipal.h"
#include "SRanipal_Eye.h"
#include "SRanipal_Enums.h"
#include <thread>
using namespace ViveSR;

// Hack to make SRanipal work.
static std::thread* SRanipalThread;
static volatile bool bPollSRanipalData;
static volatile bool bRequestStreamingStop;
static volatile bool bUseData1;
static ViveSR::anipal::Eye::EyeData EyeData1;
static ViveSR::anipal::Eye::EyeData EyeData2;
void StreamSRanipalData() 
{
	while (!bRequestStreamingStop) 
	{
		if (bPollSRanipalData)
		{
			ViveSR::anipal::Eye::EyeData eye_data;
			int result = ViveSR::anipal::Eye::GetEyeData(&eye_data);
			if (result == ViveSR::Error::WORK)
			{
				bUseData1 = !bUseData1;
				if (bUseData1)
				{
					EyeData1 = eye_data;
				}
				else
				{
					EyeData2 = eye_data;
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
// End hack

bool DecodeSRanipalBitMask(const uint64_t& bits, unsigned char position)
{
	return (bool)(bits & (uint64_t(1) << (uint64_t)position));
}

FSRanipalEyeTracker::FSRanipalEyeTracker()
	: ActivePlayerController(nullptr)
	, bSRanipalInitialized(false)
{
	// Try to initialize SRanipal.
 	int SRanipalErrorCode = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE, nullptr);
 	if (SRanipalErrorCode == ViveSR::Error::WORK)
 	{ 
 		bSRanipalInitialized = true;
		bRequestStreamingStop = false;
		bPollSRanipalData = false;

		if (SRanipalThread == nullptr)
		{
			SRanipalThread = new std::thread(StreamSRanipalData); // The SRanipal SDK simply doesn't work unless it polls on an std thread like this. FRunnables don't even work.
		}
 	}
}

FSRanipalEyeTracker::~FSRanipalEyeTracker()
{
}

void FSRanipalEyeTracker::ResetData()
{
	LeftGazeData = FSRanipalGazeData();
	RightGazeData = FSRanipalGazeData();
	CombinedGazeData = FSRanipalGazeData();
}

void FSRanipalEyeTracker::Shutdown()
{
	bRequestStreamingStop = true;
	bPollSRanipalData = false;
	GazeTrackerStatus = EEyeTrackerStatus::NotTracking;
	ResetData();

	if (bSRanipalInitialized)
	{
		ViveSR::anipal::Release(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE);
		bSRanipalInitialized = false;
	}
}

bool FSRanipalEyeTracker::Tick(float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SRanipalEyetracking_Tick);

	if (GEngine->EyeTrackingDevice.Get() != this)
	{
		Shutdown();
		return true;
	}

	if (!CVarSRanipalEnableEyetracking.GetValueOnGameThread()
		|| GEngine == nullptr
		|| GEngine->GameViewport == nullptr
		|| GEngine->GameViewport->GetWorld() == nullptr
		|| GEngine->GameViewport->GetGameViewport() == nullptr
		|| !bSRanipalInitialized)
	{
		ResetData();
		bPollSRanipalData = false;
		return true;
	}

	bPollSRanipalData = true;

	ViveSR::anipal::Eye::EyeData eye_data = bUseData1 ? EyeData1 : EyeData2;
	GazeTrackerStatus = eye_data.no_user ? EEyeTrackerStatus::Tracking : EEyeTrackerStatus::NotTracking; // This seems backwards?

	if (!ActivePlayerController.IsValid() || ActivePlayerController->GetPawn() == nullptr)
	{
		SetEyeTrackedPlayer(nullptr); // Get default
	}

	if (GazeTrackerStatus == EEyeTrackerStatus::Tracking && ActivePlayerController.IsValid())
	{
		FQuat HMDOrientation;
		FVector HMDPosition;
		GEngine->XRSystem->GetCurrentPose(IXRTrackingSystem::HMDDeviceId, HMDOrientation, HMDPosition);

		LeftGazeData.bIsGazeDataValid = false;
		RightGazeData.bIsGazeDataValid = false;

		// Find gaze direction
		bool bEitherGazeFound = false;
		FQuat ActorRotation = ActivePlayerController->GetPawn()->GetActorRotation().Quaternion();
		if (DecodeSRanipalBitMask(eye_data.verbose_data.left.eye_data_validata_bit_mask, ViveSR::anipal::Eye::SingleEyeDataValidity::SINGLE_EYE_DATA_GAZE_DIRECTION_VALIDITY))
		{
			StereoGazeData.LeftEyeDirection = HMDOrientation.RotateVector(FVector(eye_data.verbose_data.left.gaze_direction_normalized.z, -eye_data.verbose_data.left.gaze_direction_normalized.x, eye_data.verbose_data.left.gaze_direction_normalized.y));
			StereoGazeData.LeftEyeDirection = ActorRotation.RotateVector(StereoGazeData.LeftEyeDirection);
			LeftGazeData.WorldGazeDirection = StereoGazeData.LeftEyeDirection;
			LeftGazeData.bIsGazeDataValid = true;
			bEitherGazeFound = true;
		}
		if (DecodeSRanipalBitMask(eye_data.verbose_data.right.eye_data_validata_bit_mask, ViveSR::anipal::Eye::SingleEyeDataValidity::SINGLE_EYE_DATA_GAZE_DIRECTION_VALIDITY))
		{
			StereoGazeData.RightEyeDirection = HMDOrientation.RotateVector(FVector(eye_data.verbose_data.right.gaze_direction_normalized.z, -eye_data.verbose_data.right.gaze_direction_normalized.x, eye_data.verbose_data.right.gaze_direction_normalized.y));
			StereoGazeData.RightEyeDirection = ActorRotation.RotateVector(StereoGazeData.RightEyeDirection);
			RightGazeData.WorldGazeDirection = StereoGazeData.RightEyeDirection;
			RightGazeData.bIsGazeDataValid = true;
			bEitherGazeFound = true;
		}
		if (DecodeSRanipalBitMask(eye_data.verbose_data.left.eye_data_validata_bit_mask, ViveSR::anipal::Eye::SingleEyeDataValidity::SINGLE_EYE_DATA_EYE_OPENNESS_VALIDITY))
		{
			LeftGazeData.EyeOpenness = eye_data.verbose_data.left.eye_openness;
		}
		if (DecodeSRanipalBitMask(eye_data.verbose_data.right.eye_data_validata_bit_mask, ViveSR::anipal::Eye::SingleEyeDataValidity::SINGLE_EYE_DATA_EYE_OPENNESS_VALIDITY))
		{
			RightGazeData.EyeOpenness = eye_data.verbose_data.right.eye_openness;
		}
		
		// Find origin. Do not use tracker origin as it is much less useful.
		ULocalPlayer* const LocalPlayer = ActivePlayerController->GetLocalPlayer();
		if (LocalPlayer != nullptr)
		{
			const float ProjectionDistanceCm = 1000.0f;
			FSceneViewProjectionData LeftProjectionData, RightProjectionData;
			if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, eSSP_LEFT_EYE, /*out*/ LeftProjectionData))
			{
				StereoGazeData.LeftEyeOrigin = LeftGazeData.WorldGazeOrigin = LeftProjectionData.ViewOrigin;
			}
			else if (ActivePlayerController->PlayerCameraManager != nullptr)
			{
				StereoGazeData.LeftEyeOrigin = LeftGazeData.WorldGazeOrigin = ActivePlayerController->PlayerCameraManager->GetCameraLocation();
			}
			if (LocalPlayer->GetProjectionData(LocalPlayer->ViewportClient->Viewport, eSSP_RIGHT_EYE, /*out*/ RightProjectionData))
			{
				StereoGazeData.RightEyeOrigin = RightGazeData.WorldGazeOrigin = RightProjectionData.ViewOrigin;
			}
			else if (ActivePlayerController->PlayerCameraManager != nullptr)
			{
				StereoGazeData.RightEyeOrigin = RightGazeData.WorldGazeOrigin = ActivePlayerController->PlayerCameraManager->GetCameraLocation();
			}
		}
		
		// Calculate combined data
		GazeData.GazeOrigin = CombinedGazeData.WorldGazeOrigin = (StereoGazeData.LeftEyeOrigin + StereoGazeData.RightEyeOrigin) / 2.0f;
		GazeData.GazeDirection = CombinedGazeData.WorldGazeDirection = (StereoGazeData.LeftEyeDirection + StereoGazeData.RightEyeDirection) / 2.0f;
		CombinedGazeData.bIsGazeDataValid = LeftGazeData.bIsGazeDataValid || RightGazeData.bIsGazeDataValid;
		
		// Other data
		StereoGazeData.ConfidenceValue = GazeData.ConfidenceValue = bEitherGazeFound ? 1.0f : 0.0f;
		StereoGazeData.FixationPoint = GazeData.FixationPoint = FVector::ZeroVector; // Never use.
		LeftGazeData.TimeStamp = RightGazeData.TimeStamp = CombinedGazeData.TimeStamp = FDateTime::UtcNow();
	}

	if (CVarSRanipalEnableEyetrackingDebug.GetValueOnGameThread() != 0)
	{
		if (CVarSRanipalEnableGazePointDebug.GetValueOnGameThread() != 0)
		{
			FHitResult CombinedWorldGazeHitData;
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(ActivePlayerController.Get());
			CollisionQueryParams.AddIgnoredActor(ActivePlayerController->GetPawn());
			const FVector CombinedGazeFarLocation = GazeData.GazeOrigin + (GazeData.GazeDirection * 10000.0f);
			if (!ActivePlayerController->GetWorld()->LineTraceSingleByChannel(CombinedWorldGazeHitData, GazeData.GazeOrigin, CombinedGazeFarLocation, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
			{
				CombinedWorldGazeHitData.Actor = nullptr;
				CombinedWorldGazeHitData.Component = nullptr;
				CombinedWorldGazeHitData.Distance = 10000.0f;
				CombinedWorldGazeHitData.Location = CombinedGazeFarLocation;
				CombinedWorldGazeHitData.bBlockingHit = false;
			}

			UE_LOG(LogTemp, Warning, TEXT("Gaze Direction: (%.2f, %.2f, %.2f)"), GazeData.GazeDirection.X, GazeData.GazeDirection.Y, GazeData.GazeDirection.Z);
			const float DrawSize = FVector::Dist(GazeData.GazeOrigin, CombinedWorldGazeHitData.Location) * FMath::Tan(FMath::DegreesToRadians(8.0f));
			DrawDebugSphere(ActivePlayerController->GetWorld(), CombinedWorldGazeHitData.Location, DrawSize, 16, FColor::Green, false, 0.0f);
		}
	}

	return true;
}

/************************************************************************/
/* ISRanipalEyetracker                                                  */
/************************************************************************/
const FSRanipalGazeData& FSRanipalEyeTracker::GetCombinedGazeData() const
{
	return CombinedGazeData;
}

const FSRanipalGazeData& FSRanipalEyeTracker::GetLeftGazeData() const
{
	return LeftGazeData;
}

const FSRanipalGazeData& FSRanipalEyeTracker::GetRightGazeData() const
{
	return RightGazeData;
}

/************************************************************************/
/* IEyetracker                                                          */
/************************************************************************/
void FSRanipalEyeTracker::SetEyeTrackedPlayer(APlayerController* PlayerController)
{
	if (PlayerController != nullptr)
	{
		ActivePlayerController = PlayerController;
	}
	else if (GEngine != nullptr && GEngine->GameViewport != nullptr)
	{
		ActivePlayerController = GEngine->GetFirstLocalPlayerController(GEngine->GameViewport->GetWorld());
	}
}

bool FSRanipalEyeTracker::GetEyeTrackerGazeData(FEyeTrackerGazeData& OutGazeData) const
{
	OutGazeData = GazeData;
	return GazeData.ConfidenceValue > 0.5f;
}

bool FSRanipalEyeTracker::GetEyeTrackerStereoGazeData(FEyeTrackerStereoGazeData& OutGazeData) const
{
	OutGazeData = StereoGazeData;
	return OutGazeData.ConfidenceValue > 0.5f;
}

EEyeTrackerStatus FSRanipalEyeTracker::GetEyeTrackerStatus() const
{
	return GazeTrackerStatus;
}

#endif // SRANIPAL_EYETRACKING_ACTIVE
