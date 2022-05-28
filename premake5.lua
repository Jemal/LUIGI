include "vendor/premake/submodule.lua"
include "vendor/premake/version.lua"

workspace "UIEditor"
	architecture "x64"
	characterset "ASCII"
	cppdialect "c++20"
	editandcontinue "Off"
	location "code"
	platforms "x64"
	staticruntime "on"
	systemversion "latest"
	warnings "Extra"

	startproject "UIEditor"

	objdir "%{wks.location}/build/obj/%{cfg.buildcfg}"
	targetdir "%{wks.location}/build/bin/%{cfg.buildcfg}"

	configurations { 
		"Debug",
		"Release"
	}

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"No64BitChecks"
	}

	filter "configurations:Debug"
		optimize "Debug"
		symbols "on"
		defines { "DEBUG", "_DEBUG" }
	filter {}

	filter "configurations:Release"
		optimize "Speed"
		defines { "NDEBUG" }
		flags { "FatalCompileWarnings" }
	filter {}

include "code"

group "Dependencies"
	submodule.projects()
group ""