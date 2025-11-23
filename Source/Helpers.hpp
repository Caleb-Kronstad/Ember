#pragma once

#include "External.hpp"
#include "Ember.hpp"

inline void AddExternalAudio(Ember::Ember& ember)
{
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    std::filesystem::path exe_dir = std::filesystem::path(exe_path).parent_path();
    std::filesystem::path audio_dir = exe_dir / "Data/Resources/Audio";
    std::string audio_file = std::string(MAX_PATH, '\0');
            
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFile = (LPSTR)audio_file.c_str();
    ofn.nMaxFile = audio_file.size();
    ofn.lpstrInitialDir = audio_dir.string().c_str();
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrFilter = "Audio Files\0*.wav;*.mp3;*.ogg;*.flac\0WAV Files\0*.wav\0MP3 Files\0*.mp3\0OGG Files\0*.ogg\0FLAC Files\0*.flac\0All Files\0*.*\0";
    ofn.lpstrTitle = "Select audio file";
    
    if (GetOpenFileNameA(&ofn))
    {
        audio_file.resize(audio_file.find('\0'));
        std::filesystem::path absolute_path = audio_file;
        std::string abs_str = absolute_path.string();
        std::string relative_audio_path;
        size_t data_pos = abs_str.find("Data");
        if (data_pos != std::string::npos)
        {
            relative_audio_path = abs_str.substr(data_pos);
            std::replace(relative_audio_path.begin(), relative_audio_path.end(), '\\', '/');
        }
        else
            relative_audio_path = audio_file;

        std::filesystem::path path_obj(relative_audio_path);
        std::string audio_name = path_obj.stem().string();

        ember.AddAudio(audio_name, "Author's Name", relative_audio_path);
        Kiln::Log::Info("Loaded audio " + audio_name);
    }
    else
        Kiln::Log::Info("File operation cancelled");
}

inline std::string FormatTime(float seconds)
{
    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d:%02d", minutes, secs);
    return std::string(buffer);
}

inline float CalculateDeltaTime(float& last_frame_time)
{
    float currentFrameTime = static_cast<float>(glfwGetTime());
    float delta_time = currentFrameTime - last_frame_time;
    last_frame_time = currentFrameTime;
    return delta_time;
}

inline extern void SetInterfaceStyle(
    ImVec4 text_primary = ImVec4(255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f, 1.0f),
    ImVec4 background_primary = ImVec4(38.0f/255.0f, 38.0f/255.0f, 38.0f/255.0f, 1.0f),
    ImVec4 background_secondary = ImVec4(23.0f/255.0f, 23.0f/255.0f, 23.0f/255.0f, 1.0f),
    ImVec4 background_tertiary = ImVec4(56.0f/255.0f, 56.0f/255.0f, 56.0f/255.0f, 1.0f),
    ImVec4 highlight_primary = ImVec4(116.0f/255.0f, 77.0f/255.0f, 169.0f/255.0f, 1.0f),
    ImVec4 highlight_secondary = ImVec4(141.0f/255.0f, 124.0f/255.0f, 192.0f/255.0f, 1.0f)
    )
{
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = text_primary;
    colors[ImGuiCol_WindowBg] = background_primary;
    colors[ImGuiCol_Border] = background_tertiary;
    colors[ImGuiCol_FrameBg] = background_secondary;
    colors[ImGuiCol_FrameBgHovered] = background_tertiary;
    colors[ImGuiCol_FrameBgActive] = highlight_secondary;
    colors[ImGuiCol_TitleBg] = background_secondary;
    colors[ImGuiCol_TitleBgActive] = background_secondary;
    colors[ImGuiCol_TitleBgCollapsed] = background_secondary;
    colors[ImGuiCol_SliderGrab] = highlight_primary;
    colors[ImGuiCol_SliderGrabActive] = highlight_primary;
    colors[ImGuiCol_Header] = background_tertiary;
    colors[ImGuiCol_HeaderHovered] = highlight_primary;
    colors[ImGuiCol_HeaderActive] = highlight_primary;
    colors[ImGuiCol_TextSelectedBg] = highlight_primary;
    colors[ImGuiCol_CheckMark] = highlight_primary;
    colors[ImGuiCol_Button] = background_tertiary;
    colors[ImGuiCol_ButtonHovered] = highlight_secondary;
    colors[ImGuiCol_ButtonActive] = highlight_primary;
    colors[ImGuiCol_Separator] = background_tertiary;
    colors[ImGuiCol_SeparatorHovered] = highlight_primary;
    colors[ImGuiCol_SeparatorActive] = highlight_primary;
    colors[ImGuiCol_ResizeGrip] = highlight_primary;
    colors[ImGuiCol_ResizeGripHovered] = highlight_primary;
    colors[ImGuiCol_ResizeGripActive] = highlight_primary;
    colors[ImGuiCol_Tab] = background_primary;
    colors[ImGuiCol_TabHovered] = background_tertiary;
    colors[ImGuiCol_TabActive] = background_primary;
    colors[ImGuiCol_TabUnfocused] = background_primary;
    colors[ImGuiCol_TabUnfocusedActive] = background_primary;
    colors[ImGuiCol_DockingPreview] = highlight_primary;
    colors[ImGuiCol_DockingEmptyBg] = background_secondary;
    colors[ImGuiCol_PlotHistogram] = highlight_primary;
    colors[ImGuiCol_PlotHistogramHovered] = highlight_secondary;
    colors[ImGuiCol_DragDropTarget] = highlight_secondary;

    style->WindowRounding = 0.0f;
    style->FrameRounding = 0.0f;
    style->GrabRounding = 0.0f;
    style->PopupRounding = 0.0f;
    style->TabRounding = 0.0f;
    style->WindowMenuButtonPosition = ImGuiDir_Right;
    style->ScrollbarSize = 10.0f;
    style->GrabMinSize = 10.0f;
    style->DockingSeparatorSize = 1.0f;
    style->SeparatorTextBorderSize = 2.0f;
    style->WindowPadding = ImVec2(0.0f, 0.0f);
}