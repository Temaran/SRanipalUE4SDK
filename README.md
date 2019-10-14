# SRanipalUE4SDK
An SDK to get the HTC SRanipal SDK to work in UE4.

To get this to work, you must:
1. Put this plugin in your project
2. Put the SRanipal SDK includes and binaries in the right spots under the ThirdParty folder of this plugin
3. Patch the header SRanipal_EyeDataType.h header file since it doesn't compile in UE4 otherwise
4. If you want to try the social scene of the Tobii UE4 SDK, you need to patch the Sample/Common/Blueprints/BP_MirrorRobot blueprint to use SRanipal data instead of Tobii gaze data.
