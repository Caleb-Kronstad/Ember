// Copyright 2025 Caleb Kronstad

#pragma once

#include "iostream"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#ifdef KILN_PLATFORM_WINDOWS
#include <Windows.h>
#include <WinBase.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shtypes.h>
#include <ShlObj_core.h>
#include <ShlObj.h>
#endif

// ------------ -------- ------------

namespace Kiln
{
    namespace Log
    {
        template<typename T>
        void Info(T msg)
        {
            std::cout << "\x1b[37m " << "[INFO] " << msg << "\x1b[37m\n";
        }

        template<typename T>
        void Warning(T msg)
        {
            std::cout << "\x1b[33m " << "[WARNING] " << msg << "\x1b[37m\n";
        }

        template<typename T>
        void Error(T msg)
        {
            std::cout << "\x1b[31m " << "[ERROR] " << msg << "\x1b[37m\n";
        }
    }
    
    inline GLFWwindow* Initialize(const std::string& project_name = "New Kiln Project", const int& window_width = 1280, const int& window_height = 720, const bool& max_window = true)
    {
        if (!glfwInit())
        {
            Log::Error("Failed to initialize GLFW");
            return nullptr;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        int user_monitor_width = mode->width;
        int user_monitor_height = mode->height;

        GLFWwindow* window = glfwCreateWindow(window_width, window_height, project_name.c_str(), NULL, NULL);

        if (window == nullptr)
        {
            Log::Error("Failed to create GLFW window");
            return nullptr;
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Log::Error("Failed to initialize GLAD");
            return nullptr;
        }

        if (max_window)
        {
            glfwMaximizeWindow(window);
            glViewport(0, 0, user_monitor_width, user_monitor_height);
        }
        else
        {
            glViewport(0, 0, window_width, window_height);
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        return window;
    }

    inline void Begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::Begin("Buffer", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::End();
    }

    inline bool ShouldClose(GLFWwindow* window)
    {
        if (glfwWindowShouldClose(window))
            return true;
        return false;
    }

    inline void End(GLFWwindow* window)
    {
        ImGui::Render();
        ImGui::EndFrame();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    inline void Shutdown(GLFWwindow* window)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}