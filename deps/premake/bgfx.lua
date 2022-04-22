bgfx = {
	source = path.join(dependencies.base, "bgfx"),
}

function bgfx.import()
	links { "bgfx" }
	bgfx.includes()
end

function bgfx.includes()
	includedirs {
		path.join(bgfx.source, "include")
	}

	filter "configurations:debug"
		defines "BX_CONFIG_DEBUG=1"
	filter "configurations:release"
		defines "BX_CONFIG_DEBUG=0"
	filter {}
end

function bgfx.project()
	project "bgfx"
		language "C++"
		kind "StaticLib"
		
		cppdialect "C++14"
		exceptionhandling "Off"
		rtti "Off"
		warnings "Off"

		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"__STDC_FORMAT_MACROS"
		}

		includedirs {
			path.join(bx.source, "include/compat/msvc"),
			path.join(bx.source, "include"),
			path.join(bimg.source, "include"),
			path.join(bgfx.source, "include"),
			path.join(bgfx.source, "3rdparty"),
			path.join(bgfx.source, "3rdparty/dxsdk/include"),
			path.join(bgfx.source, "3rdparty/khronos")
		}

		files {
			path.join(bgfx.source, "include/bgfx/**.h"),
			path.join(bgfx.source, "src/*.cpp"),
			path.join(bgfx.source, "src/*.h")
		}
		excludes {
			path.join(bgfx.source, "src/amalgamated.cpp"),
			path.join(bgfx.source, "src/glcontext_glx.cpp"),
			path.join(bgfx.source, "src/glcontext_egl.cpp")
		}

		filter "configurations:debug"
			defines "BX_CONFIG_DEBUG=1"
		filter "configurations:release"
			defines "BX_CONFIG_DEBUG=0"
		filter {}
end

table.insert(dependencies, bgfx)