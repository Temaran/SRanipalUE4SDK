/******************************************************************************
* @author Temaran | Fredrik Lindh | temaran@gmail.com | https://github.com/Temaran
******************************************************************************/

using System;
using System.IO;
using System.Diagnostics;
using Tools.DotNETCommon;

namespace UnrealBuildTool.Rules
{
    public class SRanipalCore : ModuleRules
    {
        // If you want to remove eyetracking functionality from your builds without modifying your game code / project you can do so easily by setting this to false.
        private bool IsEyetrackingActive { get { return true; } }

        public SRanipalCore(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "Core"
                , "CoreUObject"
                , "Engine"
                , "UMG"
                , "InputCore"
                , "InputDevice"
                , "HeadMountedDisplay"
            });

            PublicDependencyModuleNames.AddRange(new string[]
            {
                "EyeTracker"
            });

            string AssemblyLocation = Path.GetDirectoryName(new Uri(System.Reflection.Assembly.GetExecutingAssembly().CodeBase).LocalPath);
            DirectoryReference RootDirectory = new DirectoryReference(Path.Combine(AssemblyLocation, "..", "..", ".."));
            bool IsEnginePlugin = RootDirectory.GetDirectoryName() == "Engine";
            PublicDefinitions.Add("SRANIPAL_COMPILE_AS_ENGINE_PLUGIN=" + (IsEnginePlugin ? 1 : 0));

            // Platform specific
            if (IsEyetrackingActive && (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32))
            {
                string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "Win64" : "Win32";
                string PluginsPath = Path.Combine(ModuleDirectory, "../../../");
                string SRanipalRelativeAPIPath = "SRanipal/ThirdParty/SRanipal";
                string SRanipalRelativeIncludePath = Path.Combine(SRanipalRelativeAPIPath, "include");
                string SRanipalRelativeLibraryBasePath = Path.Combine(SRanipalRelativeAPIPath, "lib");
                string SRanipalMainLibPath = Path.Combine(PluginsPath, SRanipalRelativeLibraryBasePath, PlatformString, "SRanipal.lib");

                // Only process SRanipal if the developer has added the appropriate binaries and includes. We only test one of them though...
                if (File.Exists(SRanipalMainLibPath))
                {
                    // Includes
                    PrivateIncludePaths.Add(Path.Combine(PluginsPath, SRanipalRelativeIncludePath));

                    // Add libraries
                    AddLibrary(SRanipalMainLibPath);

                    // Add DLL
                    string RelativeSRanipalDllPath = "";
                    string SRanipalDllName = "SRanipal.dll";

                    if (IsEnginePlugin)
                    {
                        RelativeSRanipalDllPath = Path.Combine("Binaries/ThirdParty/SRanipal", PlatformString, SRanipalDllName);
                        RuntimeDependencies.Add("$(EngineDir)/" + SRanipalDllName);
                    }
                    else
                    {
                        RelativeSRanipalDllPath = Path.Combine(SRanipalRelativeLibraryBasePath, PlatformString, SRanipalDllName);
                        RuntimeDependencies.Add(Path.Combine(PluginsPath, SRanipalDllName));
                    }

                    PublicDefinitions.Add("SRANIPAL_EYETRACKING_ACTIVE=1");
                    PublicDefinitions.Add("SRANIPAL_RELATIVE_BINARY_PATH=R\"(" + Path.Combine(SRanipalRelativeLibraryBasePath, PlatformString) + ")\"");
                    PublicDefinitions.Add("SRANIPAL_RELATIVE_DLL_PATH=R\"(" + RelativeSRanipalDllPath + ")\"");
                    PublicDelayLoadDLLs.Add(SRanipalDllName);
                    Debug.WriteLine("SRanipal SDK Build: SRanipal SDK detected!");
                }
                else
                {
                    PublicDefinitions.Add("SRANIPAL_EYETRACKING_ACTIVE=0");
                }
            }
            else
            {
                PublicDefinitions.Add("SRANIPAL_EYETRACKING_ACTIVE=0");
            }
        }

        private void AddLibrary(string libraryPath)
        {
            if (File.Exists(libraryPath))
            {
                PublicAdditionalLibraries.Add(libraryPath);
            }
            else
            {
                Debug.WriteLine("SRanipal SDK Build: Cannot find SRanipal API Lib file. Path does not exist! " + libraryPath);
            }
        }
    }
}
