dependencies = {
	base = "./deps",

	load = function()
		dir = path.join(dependencies.base, "premake/scripts/*.lua")
		deps = os.matchfiles(dir)

		for i, dep in pairs(deps) do
			dep = dep:gsub(".lua", "")
			require(dep)
		end
	end,

	imports = function()
		for i, proj in pairs(dependencies) do
			if type(i) == 'number' then
				proj.import()
			end
		end
	end,
	
	includes = function()
		for i, proj in pairs(dependencies) do
			if type(i) == 'number' then
				proj.includes()
			end
		end
	end,

	projects = function()
		for i, proj in pairs(dependencies) do
			if type(i) == 'number' then
				proj.project()
			end
		end
	end
}

dependencies.load()

workspace "luigi"
	architecture "x64"
	characterset "ASCII"
	cppdialect "c++20"
	editandcontinue "Off"
	location "./build"
	platforms "x64"
	staticruntime "on"
	symbols "on"
	systemversion "latest"
	warnings "Extra"

	objdir "%{wks.location}/obj/%{cfg.buildcfg}"
	targetdir "%{wks.location}/bin/%{cfg.buildcfg}"
	syslibdirs "%{wks.location}/bin/%{cfg.buildcfg}"

	configurations { 
		"debug",
		"release"
	}

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
		"No64BitChecks"
	}

	filter "configurations:debug"
		optimize "Debug"
		defines { "DEBUG", "_DEBUG" }
	filter {}

	filter "configurations:release"
		optimize "Speed"
		defines { "NDEBUG" }
		flags { "FatalCompileWarnings" }
	filter {}

	startproject "luigi"

	project "luigi"
		kind "WindowedApp"
		language "C++"

		warnings "Off"

		links {
			"d3d11",
		}

		pchheader "stdafx.hpp"
		pchsource "src/stdafx.cpp"

		files {
			"./src/**.hpp",
			"./src/**.cpp",
			"./src/uieditor/rc/**.*"
		}

		includedirs {
			"./src/"
		}

		debugdir "$(UIE_PATH)"
		debugcommand "$(UIE_PATH)\\$(TargetName)$(TargetExt)"

		postbuildcommands 
		{
			"copy /y \"$(OutDir)$(TargetName)$(TargetExt)\" \"$(UIE_PATH)\\$(TargetName)$(TargetExt)\"",
		}

		dependencies.imports()

	group "deps"
		dependencies.projects()