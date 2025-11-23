#pragma once

#include "External.hpp"

namespace Ember
{
    class Audio
    {
    public:
        Audio(ma_engine* engine, uint32_t id, const std::string& name, const std::string& author, const std::string& path);
        ~Audio();

        void Play();
        void Pause();
        void Stop();
        void Restart();
        
        bool Playing() const;
        bool Paused() const;
        bool Ended() const;

        ma_uint64 GetDurationInFrames() const;
        ma_uint64 GetCurrentPositionInFrames() const;
        float GetDurationInSeconds() const;
        float GetCurrentPositionInSeconds() const;

        bool SeekToFrame(ma_uint64 frame_index);
        bool SeekToTime(float time_in_seconds);

    public:
        uint32_t id;
        std::string name;
        std::string author;
        std::string path;
        ma_sound sound;
        bool initialized = false;
    };
}
