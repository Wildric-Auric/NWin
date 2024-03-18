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
		"src/",
		"vendor/GLEW/include",
		"C:/VulkanSDK/1.3.268.0/Include"
	}
	files {
		"src/*.cpp",
		"src/*.h",
		"src/*.hpp",
		"premake5.lua"
	}
	defines {
		"GLEW_STATIC"
	}
	links {
		"vendor/GLEW/lib/Release/x64/glew32s.lib",
		"C:/VulkanSDK/1.3.268.0/Lib/Vulkan-1.lib",
		"opengl32.lib",
		"gdi32.lib",
		"glu32.lib",
		"dwmapi.lib"
	}
	filter "configurations:Release"
		kind "ConsoleApp"
		optimize "On"
	filter "configurations:Debug" 
		kind "ConsoleApp"
		symbols "On"	
