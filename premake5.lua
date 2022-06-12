include "deps/premake/tools/version.lua"

dependencies = {
	base = "deps"
}

dependencies.load = function()
	local dir = path.join(dependencies.base, "premake/*.lua")
	local deps = os.matchfiles(dir)

	for i, dep in pairs(deps) do
		dep = dep:sub(1, -5) -- strip .lua from the string
		require(dep)
	end
end

dependencies.imports = function()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.import()
		end
	end
end

dependencies.projects = function()
	for i, proj in pairs(dependencies) do
		if type(i) == 'number' then
			proj.project()
		end
	end
end

dependencies.load()

workspace "UIEditor"
	architecture "x64"
	characterset "ASCII"
	cppdialect "c++20"
	editandcontinue "Off"
	location "build"
	platforms "x64"
	staticruntime "on"
	systemversion "latest"
	warnings "Extra"

	startproject "UIEditor"

	objdir "%{wks.location}/obj/%{cfg.buildcfg}"
	targetdir "%{wks.location}/bin/%{cfg.buildcfg}"

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

	project "UIEditor"
		kind "ConsoleApp"
		language "C++"

		warnings "Off"

		links {
			"d3d11"
		}

		pchheader "stdafx.hpp"
		pchsource "src/stdafx.cpp"

		files {
			"src/editor/resource/**.*",
			"src/**.hpp",
			"src/**.cpp"
		}

		includedirs {
			"src"
		}

		debugdir "$(UIE_PATH)"
		debugcommand "$(UIE_PATH)\\$(TargetName)$(TargetExt)"

		prebuildcommands {
			"pushd %{_MAIN_SCRIPT_DIR}",
			"premake5 generate-version",
			"popd"
		}

		postbuildcommands {
			"copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"$(UIE_PATH)\\$(TargetName)$(TargetExt)\"",
		}

		dependencies.imports()

group "Dependencies"
	dependencies.projects()
group ""