// Copyright 2025 Caleb Kronstad

#include "Ember.hpp"

#include "External.hpp"
#include "Helpers.hpp"
#include "Display.hpp"

#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

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
        audio_order.push_back(id);
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
        audio_order.erase(std::remove(audio_order.begin(), audio_order.end(), audio_id), audio_order.end());
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
    void Ember::MoveAudio(size_t from_index, size_t to_index)
    {
        if (from_index >= audio_order.size() || to_index >= audio_order.size()) return;
        uint32_t id = audio_order[from_index];
        audio_order.erase(audio_order.begin() + from_index);
        audio_order.insert(audio_order.begin() + to_index, id);
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
        ma_sound_set_volume(&audio->sound, volume / 100.0f);
    }
    float Ember::GetVolume() const
    {
        std::shared_ptr<Audio> audio = audios.at(current_audio_id);
        if (!audio->initialized) return 0.0f;
        return ma_sound_get_volume(&audio->sound) * 100.0f;
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

        if (json.contains("order") && json["order"].is_array())
        {
            for (auto& item : json["order"])
            {
                uint32_t id = item["id"].get<uint32_t>();
                std::string name = item["name"].get<std::string>();
                std::string author = item["author"].get<std::string>();
                std::string path = item["path"].get<std::string>();
                AddAudio(name, author, path, id);
            }
        }
        file.close();

        Kiln::Log::Info("Loaded audios from " + json_path);
        return true;
    }
    bool Ember::SaveAudios(const std::string& json_path)
    {
        nlohmann::json json;
        nlohmann::json order_json;

        for (uint32_t id : audio_order)
        {
            const auto& audio = audios.at(id);
            order_json.push_back({
                {"id", id},
                {"name", audio->name},
                {"author", audio->author},
                {"path", audio->path}
            });
        }

        json["order"] = order_json;

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
        if (ImGui::Checkbox("Loop", &ember.loop))
            ember.SetLoop(ember.loop);
        if (ImGui::SliderFloat("Volume", &ember.volume, 0.0f, 100.0f, "%.1f"))
            ember.SetVolume(ember.volume);
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Play"))
            ember.Play();
        ImGui::SameLine();
        if (ImGui::Button("Pause"))
            ember.Pause();

        static bool space_was_presseed = false;
        bool space_is_pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (space_is_pressed && !space_was_presseed)
        {
            if (ember.GetCurrentAudio()->Playing())
                ember.Pause();
            else
                ember.Play();
        }
        space_was_presseed = space_is_pressed;
	    
        float current_pos = ember.GetPosition();
        float duration = ember.GetDuration();
        if (duration > 0.0f)
        {
            float progress = current_pos / duration;
            ImGui::Separator();

            DisplayCurrentAudioInformation(ember);
        
            ImGui::Spacing();
            ImGui::Spacing();
            
            ImGui::ProgressBar(progress, ImVec2(-1, 0), (FormatTime(current_pos) + " / " + FormatTime(duration)).c_str());

            float scrub_time = current_pos;
            ImGui::PushItemWidth(-1);
            if (ImGui::SliderFloat("##Scrub", &scrub_time, 0.0f, duration, FormatTime(scrub_time).c_str()))
                ember.Seek(scrub_time);
            ImGui::PopItemWidth();
        }
        
        ImGui::Separator();

        ImGui::Spacing();
        if (ImGui::Button("+"))
            AddExternalAudio(ember);
        
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