#!lua

-- Additional Linux libs: "X11", "Xxf86vm", "Xi", "Xrandr", "stdc++"

includeDirList = {
    "../shared",
    "../shared/include",
    "../shared/gl3w",
    "../shared/imgui",
    "../shared/lodepng",
    "./irrKlang-64bit-1.6.0/include"
}

libDirectories = { 
    "../lib",
    "irrKlang-64bit-1.6.0/bin/linux-gcc-64"
}


if os.get() == "macosx" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "lodepng",
        "glfw3",
        "lua"
    }
end

if os.get() == "linux" then
    linkLibs = {
        "cs488-framework",
        "imgui",
        "lodepng",
        "glfw3",
        "lua",
        "GL",
        "Xinerama",
        "Xcursor",
        "Xxf86vm",
        "Xi",
        "Xrandr",
        "X11",
        "stdc++",
        "dl",
        "pthread",
        "openal",
        "IrrKlang"
    }
end

-- Build Options:
if os.get() == "macosx" then
    linkOptionList = { "-framework IOKit", "-framework Cocoa", "-framework CoreVideo", "-framework OpenGL" }
end

if os.get() == "linux" then
    linkOptionList = { "'-Wl,-rpath,$$ORIGIN/irrKlang-64bit-1.6.0/bin/linux-gcc-64/'" }
end

buildOptions = {"-std=c++11"}

solution "CS488-Projects"
    configurations { "Debug", "Release" }

    project "Winter"
        kind "ConsoleApp"
        language "C++"
        location "build"
        objdir "build"
        targetdir "."
        buildoptions (buildOptions)
        libdirs (libDirectories)
        links (linkLibs)
        linkoptions (linkOptionList)
        includedirs (includeDirList)
        files { "*.cpp" }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
