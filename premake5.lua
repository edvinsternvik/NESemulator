workspace "NESemulator"
    configurations { "Debug", "Release" }

project "NESemulator"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "bin-int/%{cfg.buildcfg}"

    files {
        "src/**.h",
        "src/**.cpp",
    }
    
    filter "configurations:Debug"
		defines "RNES_DEBUG"
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines "RNES_RELEASE"
		optimize "On"
		runtime "Release"