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
		path.join(dependencies.base, "stb_image"),
	}
end

function imgui.project()
	project "imgui"
		language "C++"

		includedirs {
			imgui.source,
			path.join(dependencies.base, "stb_image"),
			path.join(freetype.source, "include"),
		}

		files {
			path.join(imgui.source, "**.cpp"),
			path.join(imgui.source, "**.h"),
		}

		freetype.import()

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, imgui)