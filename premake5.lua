workspace "NWin"
	configurations {"Release", "Debug"}
	platforms {"x86", "x64"}

	filter {"platforms:x86"}
		system "Windows"
		architecture "x86"
	filter{}
	filter {"platforms:x64"}
		system "Windows"
		architecture "x86_64"
	filter{}

	language "C++"
    cppdialect "C++11"
	targetdir "build/%{prj.name}/%{cfg.buildcfg}"
    objdir    "build/objs"
    characterset("MBCS")
    buildoptions { }
	location "./"
	project "Window"
	includedirs {
		"src/"
	}
	files {
		"src/*.cpp",
		"src/*.h",
		"src/*.hpp",
		"premake5.lua"
	}
	links {
		"dwmapi.lib"
	}
	filter "configurations:Release"
		kind "ConsoleApp"
		optimize "On"
	filter "configurations:Debug" 
		kind "ConsoleApp"
		symbols "On"	
