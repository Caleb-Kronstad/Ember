// Copyright 2025 Caleb Kronstad

#include "Ember.hpp"

#include "External.hpp"
#include "Helpers.hpp"

namespace Ember
{
    template<typename T>
    T Clamp(T value, T min, T max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
    
    Ember::Ember()
    {
    }
    Ember::~Ember()
    {
    }

    void Ember::Begin()
    {
        engine_config = ma_engine_config_init();
        ma_result result = ma_engine_init(&engine_config, &engine);
        if (result != MA_SUCCESS)
        {
            Kiln::Log::Error("Unable to initialize audio engine");
            initialized = false;
            return;
        }

        ma_engine_set_volume(&engine, 1.0f);
        initialized = true;
    }
    void Ember::End()
    {
        audios.clear();
        if (initialized)
        {
            ma_engine_uninit(&engine);
            initialized = false;
        }
    }

    bool Ember::AddAudio(const std::string& name, const std::string& author, const std::string& path, uint32_t id)
    {
        if (id == 0)
        {
            id = 10000000;
            if (!audios.empty())
            {
                auto max_it = std::max_element(
                    audios.begin(),
                    audios.end(),
                    [](const auto& a, const auto& b) { return a.first < b.first; }
                );
                id = max_it->first + 1;
            }
        }
        std::shared_ptr<Audio> new_audio = std::make_shared<Audio>(&engine, id, name, author, path);
        audios.insert_or_assign(id, new_audio);
        return true;
    }
    bool Ember::RemoveAudio(uint32_t audio_id)
    {
        if (!audios.contains(audio_id))
        {
            Kiln::Log::Error("Unable to find audio of id " + std::to_string(audio_id));
            return false;
        }
        audios.erase(audio_id);
        return true;
    }
    bool Ember::SetAudio(uint32_t audio_id)
    {
        if (!audios.contains(audio_id))
        {
            Kiln::Log::Error("Unable to find audio of id " + std::to_string(audio_id));
            return false;
        }
        Stop();
        current_audio_id = audio_id;
        SetVolume(volume);
        SetLoop(loop);
        Play();
        return true;
    }

    void Ember::Play()
    {
        audios.at(current_audio_id)->Play();
    }
    void Ember::Pause()
    {
        audios.at(current_audio_id)->Pause();
    }
    void Ember::Stop()
    {
        audios.at(current_audio_id)->Stop();
    }
    
    void Ember::SetVolume(float vol)
    {
        volume = vol;
        std::shared_ptr<Audio> audio = audios.at(current_audio_id);
        if (!audio->initialized) return;
        ma_sound_set_volume(&audio->sound, Clamp(volume, 0.0f, 1.0f));
    }
    float Ember::GetVolume() const
    {
        std::shared_ptr<Audio> audio = audios.at(current_audio_id);
        if (!audio->initialized) return 0.0f;
        return ma_sound_get_volume(&audio->sound);
    }
    void Ember::SetLoop(bool looping)
    {
        loop = looping;
        std::shared_ptr<Audio> audio = audios.at(current_audio_id);
        if (!audio->initialized) return;
        ma_sound_set_looping(&audio->sound, loop);
    }
    bool Ember::GetLoop() const
    {
        std::shared_ptr<Audio> audio = audios.at(current_audio_id);
        if (!audio->initialized) return false;
        return ma_sound_is_looping(&audio->sound) == MA_TRUE;
    }

    float Ember::GetDuration() const
    {
        if (audios.empty() || !audios.contains(current_audio_id))
            return 0.0f;
        return audios.at(current_audio_id)->GetDurationInSeconds();
    }

    float Ember::GetPosition() const
    {
        if (audios.empty() || !audios.contains(current_audio_id))
            return 0.0f;
        return audios.at(current_audio_id)->GetCurrentPositionInSeconds();
    }

    bool Ember::Seek(float time_in_seconds)
    {
        if (audios.empty() || !audios.contains(current_audio_id))
            return false;
        return audios.at(current_audio_id)->SeekToTime(time_in_seconds);
    }

    bool Ember::RenameAudio(uint32_t audio_id, const std::string& new_name, const std::string& new_author)
    {
        if (!audios.contains(audio_id))
        {
            Kiln::Log::Error("Unable to find audio of id " + std::to_string(audio_id));
            return false;
        }

        audios.at(audio_id)->name = new_name;
        audios.at(audio_id)->author = new_author;
        return true;
    }

    bool Ember::LoadAudios(const std::string& json_path)
    {
        std::ifstream file(json_path);
        if (!file.is_open())
        {
            Kiln::Log::Error("Unable to open file " + json_path);
            return false;
        }

        nlohmann::json json;
        try
        {
            file >> json;
        }
        catch (const nlohmann::json::exception& e)
        {
            Kiln::Log::Error("Unable to parse JSON file " + std::string(e.what()));
            return false;
        }

        for (auto& [key, value] : json.items())
        {
            uint32_t id = std::stoul(key);
            std::string name = value["name"].get<std::string>();
            std::string author = value["author"].get<std::string>();
            std::string path = value["path"].get<std::string>();
            AddAudio(name, author, path, id);
        }
        file.close();

        Kiln::Log::Info("Loaded audios from " + json_path);
        return true;
    }
    bool Ember::SaveAudios(const std::string& json_path)
    {
        nlohmann::json json;

        for (const auto& [key, value] : audios)
        {
            std::string id = std::to_string(key);
            json[id] = {
                {"name", value->name},
                {"author", value->author},
                {"path", value->path}
            };
        }

        std::ofstream file(json_path);
        if (!file.is_open())
        {
            Kiln::Log::Error("Unable to open file " + json_path);
            return false;
        }
        try
        {
            file << json.dump(4);
        }
        catch (const nlohmann::json::exception& e)
        {
            Kiln::Log::Error("Unable to write JSON file " + std::string(e.what()));
            return false;
        }
        file.close();

        Kiln::Log::Info("Saved audios to " + json_path);
        return true;
    }

    bool Ember::LoadSettings(const std::string& json_path)
    {
        std::ifstream file(json_path);
        if (!file.is_open())
        {
            Kiln::Log::Error("Unable to open file " + json_path);
            return false;
        }

        nlohmann::json json;
        try
        {
            file >> json;
        }
        catch (const nlohmann::json::exception& e)
        {
            Kiln::Log::Error("Unable to parse JSON file " + std::string(e.what()));
            return false;
        }

        loop = json["loop"].get<bool>();
        volume = json["volume"].get<float>();
        current_audio_id = json["last-played"].get<uint32_t>();
        SetVolume(volume);
        SetLoop(loop);

        Kiln::Log::Info("Loaded settings from " + json_path);
        return true;
    }
    bool Ember::SaveSettings(const std::string& json_path)
    {
        nlohmann::json json;

        json["loop"] = loop;
        json["volume"] = volume;
        json["last-played"] = current_audio_id;

        std::ofstream file(json_path);
        if (!file.is_open())
        {
            Kiln::Log::Error("Unable to open file " + json_path);
            return false;
        }
        try
        {
            file << json.dump(4);
        }
        catch (const nlohmann::json::exception& e)
        {
            Kiln::Log::Error("Unable to write JSON file " + std::string(e.what()));
            return false;
        }
        file.close();

        Kiln::Log::Info("Saved settings to " + json_path);
        return true;
    }
}









bool display_song_info = false;
bool show_rename_popup = false;
uint32_t rename_audio_id = 0;
char rename_buffer_name[256] = "";
char rename_buffer_author[256] = "";

void AddExternalAudio(Ember::Ember& ember)
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

void DisplayAudios(Ember::Ember& ember)
{
    for (auto& [id, audio] : ember.GetAudios())
    {
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::PushID(id);
            
        if (ImGui::Button(audio->name.c_str()))
            ember.SetAudio(audio->id);

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

        if (display_song_info)
        {
            ImGui::Text(("Author: " + audio->author).c_str());
            ImGui::Text(("Path: " + audio->path).c_str());
            ImGui::Text(("ID: " + std::to_string(id)).c_str());
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

std::string FormatTime(float seconds)
{
    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;

    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%d:%02d", minutes, secs);
    return std::string(buffer);
}

float CalculateDeltaTime(float& last_frame_time)
{
    float currentFrameTime = static_cast<float>(glfwGetTime());
    float delta_time = currentFrameTime - last_frame_time;
    last_frame_time = currentFrameTime;
    return delta_time;
}




int main()
{
    GLFWwindow* window = Kiln::Initialize("Ember");
    Ember::Ember ember = Ember::Ember();
    float last_frame_time = 0.0f;
    
    ember.Begin();
    ember.LoadAudios("Files/audios.ember");
    ember.LoadSettings("Files/settings.ember");
    SetInterfaceStyle();
    
    bool window_closed = false;
    while (!window_closed)
    {
        float delta_time = CalculateDeltaTime(last_frame_time);
        Kiln::Begin();
        
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
        ImGui::Begin("Ember", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::Indent(16.0f);
        
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        
        ImGui::PushItemWidth(200.0f);
        if (ImGui::SliderFloat("Volume", &ember.volume, 0.0f, 1.0f))
            ember.SetVolume(ember.volume);
        ImGui::SameLine();
        if (ImGui::Checkbox("Loop", &ember.loop))
            ember.SetLoop(ember.loop);
        ImGui::SameLine();
        ImGui::Checkbox("Display Information", &display_song_info);

        if (ImGui::Button("Play"))
            ember.Play();
        ImGui::SameLine();
        if (ImGui::Button("Pause"))
            ember.Pause();
        ImGui::SameLine();
        if (ImGui::Button("+"))
            AddExternalAudio(ember);
	    
        float currentPos = ember.GetPosition();
        float duration = ember.GetDuration();
        if (duration > 0.0f)
        {
            float progress = currentPos / duration;
            ImGui::Separator();
            ImGui::Text("Progress");
            ImGui::ProgressBar(progress, ImVec2(-1, 0), (FormatTime(currentPos) + " / " + FormatTime(duration)).c_str());

            float scrubTime = currentPos;
            ImGui::PushItemWidth(-1);
            if (ImGui::SliderFloat("##Scrub", &scrubTime, 0.0f, duration, FormatTime(scrubTime).c_str()))
                ember.Seek(scrubTime);
            ImGui::PopItemWidth();
        }
        
        ImGui::Separator();
        DisplayAudios(ember);
        
        ImGui::PopItemWidth();
        ImGui::Unindent();
        ImGui::End();

        Kiln::End(window);
        window_closed = Kiln::ShouldClose(window);
    }

    ember.SaveAudios("Files/audios.ember");
    ember.SaveSettings("Files/settings.ember");
    ember.End();
    Kiln::Shutdown(window);
    return 0;
}