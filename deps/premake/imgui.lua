imgui = {
	source = path.join(dependencies.base, "imgui"),
}

function imgui.import()
	links { "imgui" }
	imgui.includes()
end

function imgui.includes()
	print("retard")
	print(imgui.source)

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
			path.join(imgui.source, "backends/imgui_impl_dx11.cpp"),
			path.join(imgui.source, "backends/imgui_impl_win32.cpp"),
			path.join(imgui.source, "backends/imgui_impl_win32.h"),
			path.join(imgui.source, "misc/freetype/imgui_freetype.cpp"),
			path.join(imgui.source, "misc/freetype/imgui_freetype.h"),
			path.join(imgui.source, "misc/imgui_uie.cpp"),
			path.join(imgui.source, "misc/imgui_uie.h")
		}

		freetype.import()

		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, imgui)