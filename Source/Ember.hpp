#pragma once
#include <memory>

#include "Audio.hpp"

namespace Ember
{
    class Ember
    {
    public:
        Ember();
        ~Ember();

        void Begin();
        void End();

        bool AddAudio(const std::string& name, const std::string& author, const std::string& path, uint32_t id = 0);
        bool RemoveAudio(uint32_t audio_id);
        bool SetAudio(uint32_t audio_id);

        void Play();
        void Pause();
        void Stop();

        void SetVolume(float volume);
        float GetVolume() const;
        void SetLoop(bool loop);
        bool GetLoop() const;
        
        ma_engine* GetEngine() { return &engine; }

        bool LoadAudios(const std::string& json_path);
        bool SaveAudios(const std::string& json_path);
        bool LoadSettings(const std::string& json_path);
        bool SaveSettings(const std::string& json_path);

        float GetDuration() const;
        float GetPosition() const;
        bool Seek(float time_in_seconds);

        bool RenameAudio(uint32_t audio_id, const std::string& new_name, const std::string& new_author);
        const std::unordered_map<uint32_t, std::shared_ptr<Audio>>& GetAudios() { return audios; }

    public:
        bool loop = false;
        float volume = 1.0f;

    private:
        std::unordered_map<uint32_t, std::shared_ptr<Audio>> audios;
        uint32_t current_audio_id;
        ma_engine engine;
        ma_engine_config engine_config;
        bool initialized = false;
    };
}
