function generate_version()
	local proc = assert(io.popen("git rev-list --count HEAD", "r"))
	local revision = assert(proc:read("*a")):gsub("%s+", "")
	proc:close()

	local versionFilePath = "src/version.hpp"

	local versionFile = assert(io.open(versionFilePath, "w"))
	versionFile:write("#pragma once\n\n")
	versionFile:write("#define REVISION_NUMBER " .. revision .. "\n\n")
	versionFile:write("#define REVISION_NUMBER_STR \"" .. revision .. "\"\n\n")
	versionFile:close()
end

newaction {
	trigger = "generate-version",
	description = "Generate version file for project",
	
	onWorkspace = function (wks)
		generate_version()
	end
}