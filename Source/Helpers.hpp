#pragma once

#include "External.hpp"

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