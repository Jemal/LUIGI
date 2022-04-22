bimg = {
	source = path.join(dependencies.base, "bimg"),
}

function bimg.import()
	links { "bimg" }
	bimg.includes()
end

function bimg.includes()
	includedirs {
		path.join(bimg.source, "include")
	}

	filter "configurations:debug"
		defines "BX_CONFIG_DEBUG=1"
	filter "configurations:release"
		defines "BX_CONFIG_DEBUG=0"
	filter {}
end

function bimg.project()
	project "bimg"
		language "C++"
		kind "StaticLib"
	
		cppdialect "C++14"
		exceptionhandling "Off"
		rtti "Off"
		warnings "Off"

		includedirs {
			path.join(bx.source, "include/compat/msvc"),
			path.join(bx.source, "include"),
			path.join(bimg.source, "include"),
			path.join(bimg.source, "3rdparty/astc-codec"),
			path.join(bimg.source, "3rdparty/astc-codec/include")
		}

		files {
			path.join(bimg.source, "include/bimg/*.h"),
			path.join(bimg.source, "src/image.cpp"),
			path.join(bimg.source, "src/image_gnf.cpp"),
			path.join(bimg.source, "src/*.h"),
			path.join(bimg.source, "3rdparty/astc-codec/src/decoder/*.cc")
		}

		filter "configurations:debug"
			defines "BX_CONFIG_DEBUG=1"
		filter "configurations:release"
			defines "BX_CONFIG_DEBUG=0"
		filter {}
end

table.insert(dependencies, bimg)