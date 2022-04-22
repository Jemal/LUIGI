bx = {
	source = path.join(dependencies.base, "bx"),
}

function bx.import()
	links { "bx" }
	bx.includes()
end

function bx.includes()
	includedirs {
		path.join(bx.source, "include"),
		path.join(bx.source, "include/compat/msvc")
	}

	filter "configurations:debug"
		defines "BX_CONFIG_DEBUG=1"
	filter "configurations:release"
		defines "BX_CONFIG_DEBUG=0"
	filter {}
end

function bx.project()
	project "bx"
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
			path.join(bx.source, "3rdparty"),
			path.join(bx.source, "include")
		}

		files {
			path.join(bx.source, "include/bx/*.h"),
			path.join(bx.source, "include/bx/inline/*.inl"),
			path.join(bx.source, "src/*.cpp")
		}
		excludes {
			path.join(bx.source, "src/amalgamated.cpp"),
			path.join(bx.source, "src/crtnone.cpp")
		}

		filter "configurations:debug"
			defines "BX_CONFIG_DEBUG=1"
		filter "configurations:release"
			defines "BX_CONFIG_DEBUG=0"
		filter {}
end

table.insert(dependencies, bx)