#pragma once

#include "External.hpp"
#include "Ember.hpp"

inline bool show_rename_popup = false;
inline uint32_t rename_audio_id = 0;
inline char rename_buffer_name[256] = "";
inline char rename_buffer_author[256] = "";
inline int drag_source_index = -1;

inline void DisplayAudios(Ember::Ember& ember)
{
    const auto& audio_order = ember.GetAudioOrder();
    const auto& audios = ember.GetAudios();

    for (size_t i = 0; i < audio_order.size(); i++)
    {
        uint32_t id = audio_order[i];
        auto& audio = audios.at(id);

        ImGui::Spacing();
        ImGui::PushID(id);

        if (ImGui::Button(audio->name.c_str()))
        {
            ember.SetAudio(audio->id);
            ember.GetCurrentAudio()->Restart();
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            ImGui::SetDragDropPayload("AudioReorder", &i, sizeof(size_t));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AudioReorder"))
            {
                size_t from_index = *(const size_t*)payload->Data;
                ember.MoveAudio(from_index, i);
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Rename"))
            {
                rename_audio_id = id;
                strncpy_s(rename_buffer_name, audio->name.c_str(), sizeof(rename_buffer_name) - 1);
                strncpy_s(rename_buffer_author, audio->author.c_str(), sizeof(rename_buffer_author) - 1);
                show_rename_popup = true;
            }
            ImGui::EndPopup();
        }

        ImGui::PopID();
    }

    if (show_rename_popup)
    {
        ImGui::OpenPopup("Rename Audio");
        show_rename_popup = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(116.0f/255.0f, 77.0f/255.0f, 169.0f/255.0f, 1.0f));
    if (ImGui::BeginPopupModal("Rename Audio", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(38.0f/255.0f, 38.0f/255.0f, 38.0f/255.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(56.0f/255.0f, 56.0f/255.0f, 56.0f/255.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(141.0f/255.0f, 124.0f/255.0f, 192.0f/255.0f, 1.0f));
        ImGui::Text("Name:");
        ImGui::InputText("##name", rename_buffer_name, sizeof(rename_buffer_name));

        ImGui::Spacing();
        ImGui::Text("Author:");
        ImGui::InputText("##author", rename_buffer_author, sizeof(rename_buffer_author));
        ImGui::PopStyleColor(3);

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Apply", ImVec2(120, 0)))
        {
            ember.RenameAudio(rename_audio_id,
                             std::string(rename_buffer_name),
                             std::string(rename_buffer_author));
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleColor();
}

inline void DisplayCurrentAudioInformation(Ember::Ember& ember)
{
    std::shared_ptr<Ember::Audio> current_audio = ember.GetCurrentAudio();
    
    ImGui::Text(current_audio->name.c_str());
    ImGui::SameLine(); ImGui::TextColored(ImVec4(141.0f/255.0f, 124.0f/255.0f, 192.0f/255.0f, 1.0f), std::to_string(current_audio->id).c_str());
    ImGui::Text(current_audio->author.c_str());
    ImGui::Text(current_audio->path.c_str());
}