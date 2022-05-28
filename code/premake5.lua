project "UIEditor"
	kind "ConsoleApp"
	language "C++"

	warnings "Off"

	links {
		"d3d11",
		"d3dcompiler"
	}

	pchheader "stdafx.hpp"
	pchsource "src/stdafx.cpp"

	files {
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"src/"
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

	submodule.include({
		"imgui",
		"freetype",
	})