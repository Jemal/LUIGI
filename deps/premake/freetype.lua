freetype = {
	source = path.join(dependencies.base, "freetype"),
}

function freetype.import()
	links { "freetype" }
	freetype.includes()
end

function freetype.includes()
	includedirs {
		path.join(freetype.source, "include")
	}

	defines { "FT2_BUILD_LIBRARY", "FT_OPTION_AUTOFIT2" }
end

function freetype.project()
	project "freetype"
		language "C"

		warnings "Off"
		kind "StaticLib"

		freetype.includes()

		files {
			path.join(freetype.source, "src/base/ftdebug.c"),
			path.join(freetype.source, "src/base/ftbase.c"),
			path.join(freetype.source, "src/base/ftbbox.c"),
			path.join(freetype.source, "src/base/ftbitmap.c"),
			path.join(freetype.source, "src/base/ftgasp.c"),
			path.join(freetype.source, "src/base/ftglyph.c"),
			path.join(freetype.source, "src/base/ftinit.c"),
			path.join(freetype.source, "src/base/ftstroke.c"),
			path.join(freetype.source, "src/base/ftsynth.c"),
			path.join(freetype.source, "src/base/ftsystem.c"),
			path.join(freetype.source, "src/base/fttype1.c"),
			path.join(freetype.source, "src/base/ftotval.c"),

			path.join(freetype.source, "src/autofit/autofit.c"),
			path.join(freetype.source, "src/bdf/bdf.c"),
			path.join(freetype.source, "src/cff/cff.c"),
			path.join(freetype.source, "src/cid/type1cid.c"),
			path.join(freetype.source, "src/lzw/ftlzw.c"),
			path.join(freetype.source, "src/psaux/psaux.c"),
			path.join(freetype.source, "src/psnames/psnames.c"),
			path.join(freetype.source, "src/smooth/smooth.c"),
			path.join(freetype.source, "src/sfnt/sfnt.c"),
			path.join(freetype.source, "src/pcf/pcf.c"),
			path.join(freetype.source, "src/pfr/pfr.c"),
			path.join(freetype.source, "src/truetype/truetype.c"),
			path.join(freetype.source, "src/type1/type1.c"),
			path.join(freetype.source, "src/type42/type42.c"),
			path.join(freetype.source, "src/raster/raster.c"),
			path.join(freetype.source, "src/sdf/sdf.c"),
			path.join(freetype.source, "src/otvalid/otvalid.c"),
			path.join(freetype.source, "src/pshinter/pshinter.c"),
			path.join(freetype.source, "src/winfonts/winfnt.c"),
			path.join(freetype.source, "src/gzip/ftgzip.c"),
			path.join(freetype.source, "src/gzip/zutil.c")
		}
end

table.insert(dependencies, freetype)