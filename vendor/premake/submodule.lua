submodule = {}
submodule.list = {}
submodule.path = "vendor/"

submodule.define = function( dep )
	table.insert(submodule.list, dep)
end

submodule.include = function( list )
	for _, name in pairs(list) do
		local dep = {}

		for i, v in ipairs(submodule.list) do
			if v.name == name then
				dep = v
				break
			end
		end

		if dep ~= nil then
			if dep.type ~= "include" then
				dependson(name)
				links(name)
			end

			if dep.defines ~= nil then
				for i, define in pairs(dep.defines) do
					defines { define }
				end
			end

			for i, include in pairs(dep.includes) do
				includedirs { 
					path.join("../", include) 
				}
			end
		end
	end
end

submodule.projects = function()
	for i, dep in pairs(submodule.list) do
		if dep.type ~= "include" then
			project(dep.name)
			language(dep.language)
			kind(dep.type)
			location(submodule.path)
		
			warnings "Off"

			-- we should always have includes
			for i, include in pairs(dep.includes) do
				includedirs { include }
			end

			if dep.files ~= nil then
				for i, file in pairs(dep.files) do
					files { file }
				end
			end

			if dep.defines ~= nil then
				for i, define in pairs(dep.defines) do
					defines { define }
				end
			end
		end
	end
end

-- !! DEFINE SUBMODULES BELOW !!

submodule.define({
	name = "entt",
	language = "C++",
	type = "include",
	includes = {
		"vendor/entt"
	}
})

submodule.define({
	name = "freetype",
	language = "C",
	type = "StaticLib",
	includes = {
		"vendor/freetype/include"
	},
	files = {
		"vendor/freetype/src/base/ftdebug.c",
		"vendor/freetype/src/base/ftbase.c",
		"vendor/freetype/src/base/ftbbox.c",
		"vendor/freetype/src/base/ftbitmap.c",
		"vendor/freetype/src/base/ftgasp.c",
		"vendor/freetype/src/base/ftglyph.c",
		"vendor/freetype/src/base/ftinit.c",
		"vendor/freetype/src/base/ftstroke.c",
		"vendor/freetype/src/base/ftsynth.c",
		"vendor/freetype/src/base/ftsystem.c",
		"vendor/freetype/src/base/fttype1.c",
		"vendor/freetype/src/base/ftotval.c",

		"vendor/freetype/src/autofit/autofit.c",
		"vendor/freetype/src/bdf/bdf.c",
		"vendor/freetype/src/cff/cff.c",
		"vendor/freetype/src/cid/type1cid.c",
		"vendor/freetype/src/lzw/ftlzw.c",
		"vendor/freetype/src/psaux/psaux.c",
		"vendor/freetype/src/psnames/psnames.c",
		"vendor/freetype/src/smooth/smooth.c",
		"vendor/freetype/src/sfnt/sfnt.c",
		"vendor/freetype/src/pcf/pcf.c",
		"vendor/freetype/src/pfr/pfr.c",
		"vendor/freetype/src/truetype/truetype.c",
		"vendor/freetype/src/type1/type1.c",
		"vendor/freetype/src/type42/type42.c",
		"vendor/freetype/src/raster/raster.c",
		"vendor/freetype/src/sdf/sdf.c",
		"vendor/freetype/src/otvalid/otvalid.c",
		"vendor/freetype/src/pshinter/pshinter.c",
		"vendor/freetype/src/winfonts/winfnt.c",
		"vendor/freetype/src/gzip/ftgzip.c",
		"vendor/freetype/src/gzip/zutil.c"
	},
	defines = {
		"FT2_BUILD_LIBRARY",
		"FT_OPTION_AUTOFIT2"
	}
})

submodule.define({
	name = "imgui",
	language = "C++",
	type = "StaticLib",
	includes = {
		"vendor/imgui",
		"vendor/freetype/include"
	},
	files = {
		"vendor/imgui/**.cpp",
		"vendor/imgui/**.h"
	}
})

submodule.define({
	name = "stb_image",
	language = "C++",
	type = "include",
	includes = {
		"vendor/stb_image"
	}
})