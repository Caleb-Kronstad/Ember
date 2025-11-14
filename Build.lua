workspace "Ember"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Ember"

project "Kiln"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"
    
    files {
        "Includes/Kiln.hpp",
        -- glad
        "Includes/glad/include/glad/glad.h",
        "Includes/glad/include/KHR/khrplatform.h",
        "Includes/glad/src/glad.cpp",
        -- glfw
        "Includes/glfw/include/GLFW/glfw3.h",
        "Includes/glfw/include/GLFW/glfw3native.h",
        "Includes/glfw/src/glfw_config.h",
        "Includes/glfw/src/context.c",
        "Includes/glfw/src/init.c",
        "Includes/glfw/src/input.c",
        "Includes/glfw/src/monitor.c",
        "Includes/glfw/src/vulkan.c",
        "Includes/glfw/src/window.c",
        "Includes/glfw/src/egl_context.c",
        "Includes/glfw/src/osmesa_context.c",
        -- imgui
        "Includes/imgui/*.cpp",
        "Includes/imgui/*.h",
        "Includes/imgui/misc/cpp/*.cpp",
        "Includes/imgui/misc/cpp/*.h",
        "Includes/imgui/backends/imgui_impl_glfw.h",
        "Includes/imgui/backends/imgui_impl_opengl3.h",
        "Includes/imgui/backends/imgui_impl_glfw.cpp",
        "Includes/imgui/backends/imgui_impl_opengl3.cpp"
        }
    
    includedirs {
        "Includes/glad/include",
        "Includes/glfw/include",
        "Includes/imgui"
        }
    
    defines {
        "IMGUI_IMPL_OPENGL_LOADER_GLAD"
        }
    
    targetdir ("%{wks.location}/Library/%{prj.name}")
    objdir ("%{wks.location}/Intermediate/%{prj.name}")
    
    filter "system:windows"
        systemversion "latest"
        defines { 
            "KILN_PLATFORM_WINDOWS",
            "_GLFW_WIN32", 
            "_CRT_SECURE_NO_WARNINGS"
            }
        links {
            "opengl32",
            "comdlg32",
            "gdi32"
        }
            
    filter "system:linux"
        pic "On"
        staticruntime "On"
        defines { 
            "_GLFW_X11",
            "KILN_PLATFORM_LINUX" 
            }
        links {
            "GL",
            "dl",
            "pthread",
            "X11",
            "Xrandr",
            "Xi",
            "Xxf86vm",
            "Xcursor",
            "Xinerama"
        }
    
    filter "configurations:Debug"
            defines { "DEBUG" }
            runtime "Debug"
            symbols "On"
            
    filter "configurations:Release"
            defines { "RELEASE" }
            runtime "Release"
            optimize "On"
            symbols "On"
            

project "Ember"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"
    
    files {
        "Source/**.hpp",
        "Source/**.cpp",
        "Includes/miniaudio/miniaudio.h",
        "Includes/miniaudio/miniaudio.c",
        "Includes/json/json.hpp"
        }
    
    includedirs {
        "Includes",
        "Includes/glad/include",
        "Includes/glfw/include",
        "Includes/imgui",
        "Includes/miniaudio",
        "Includes/json"
        }
    
    links {
        "Kiln"
        }
    
    targetdir ("%{wks.location}/Library/%{prj.name}")
    objdir ("%{wks.location}/Intermediate/%{prj.name}")
    
    filter "system:windows"
        systemversion "latest"
        files {
            "Includes/glfw/src/win32_init.c",
            "Includes/glfw/src/win32_joystick.c",
            "Includes/glfw/src/win32_monitor.c",
            "Includes/glfw/src/win32_time.c",
            "Includes/glfw/src/win32_thread.c",
            "Includes/glfw/src/win32_window.c",
            "Includes/glfw/src/wgl_context.c"
        }
        defines { 
            "KILN_PLATFORM_WINDOWS",
            "_GLFW_WIN32", 
            "_CRT_SECURE_NO_WARNINGS"
            }
        links {
            "opengl32",
            "comdlg32",
            "gdi32"
        }
        
    filter "system:linux"
        pic "On"
        staticruntime "On"
        defines { 
            "_GLFW_X11",
            "KILN_PLATFORM_LINUX" 
            }
        links {
            "GL",
            "dl",
            "pthread",
            "X11",
            "Xrandr",
            "Xi",
            "Xxf86vm",
            "Xcursor",
            "Xinerama"
        }
        files {
             "Includes/glfw/src/x11_init.c",
             "Includes/glfw/src/x11_monitor.c",
             "Includes/glfw/src/x11_window.c",
             "Includes/glfw/src/xkb_unicode.c",
             "Includes/glfw/src/posix_time.c",
             "Includes/glfw/src/posix_thread.c",
             "Includes/glfw/src/glx_context.c",
             "Includes/glfw/src/linux_joystick.c"
         }
    
    filter "configurations:Debug"
            defines { "DEBUG" }
            runtime "Debug"
            symbols "On"
            
    filter "configurations:Release"
            defines { "RELEASE" }
            runtime "Release"
            optimize "On"
            symbols "On"