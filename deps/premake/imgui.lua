imgui = {
	source = path.join(dependencies.base, "imgui"),
}

function imgui.import()
	links { "imgui" }
	imgui.includes()
end

function imgui.includes()
	includedirs {
		imgui.source,
		path.join(imgui.source, "stb"),
	}
end

function imgui.project()
	project "imgui"
		language "C++"

		includedirs {
			imgui.source,
			path.join(imgui.source, "stb"),
			path.join(freetype.source, "include"),
			path.join(freetype.source, "include"),
		}

		files {
			path.join(imgui.source, "*.cpp"),
			path.join(imgui.source, "*.h"),
			path.join(imgui.source, "backends/**.cpp"),
			path.join(imgui.source, "backends/**.h"),
			path.join(imgui.source, "misc/**.cpp"),
			path.join(imgui.source, "misc/**.h"),
			path.join(imgui.source, "misc/**.hpp")
		}

		freetype.import()

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, imgui)