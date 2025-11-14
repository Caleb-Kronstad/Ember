#include "Audio.hpp"

#include "External.hpp"

namespace Ember
{
    Audio::Audio(ma_engine* engine, uint32_t id, const std::string& name, const std::string& author, const std::string& path)
        : id(id), name(name), author(author), path(path)
    {
        if (engine == nullptr)
        {
            Kiln::Log::Error("Unable to find audio engine, cannot create audio");
            return;
        }

        ma_result result = ma_sound_init_from_file(engine, path.c_str(), MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_ASYNC, NULL, NULL, &sound);
        if (result != MA_SUCCESS)
        {
            Kiln::Log::Error("Unable to load audio file " + path);
            return;
        }

        ma_sound_set_spatialization_enabled(&sound, MA_TRUE);
        initialized = true;
    }
    Audio::~Audio()
    {
        if (initialized)
            ma_sound_uninit(&sound);
    }

    void Audio::Play()
    {
        if (!initialized) return;
        ma_sound_start(&sound);
    }

    void Audio::Pause()
    {
        if (!initialized) return;
        ma_sound_stop(&sound);
    }

    void Audio::Stop()
    {
        if (!initialized) return;
        ma_sound_stop(&sound);
    }

    bool Audio::Playing() const
    {
        if (!initialized) return false;
        return ma_sound_is_playing(&sound) == MA_TRUE;
    }

    bool Audio::Paused() const
    {
        if (!initialized) return false;
        return !Playing() && !Ended();
    }

    bool Audio::Ended() const
    {
        if (!initialized) return false;
        return ma_sound_at_end(&sound) == MA_TRUE;
    }

    ma_uint64 Audio::GetDurationInFrames() const
    {
        if (!initialized) return 0;
        ma_uint64 length = 0;
        ma_sound_get_length_in_pcm_frames(&sound, &length);
        return length;
    }

    ma_uint64 Audio::GetCurrentPositionInFrames() const
    {
        if (!initialized) return 0;
        ma_uint64 cursor = 0;
        ma_sound_get_cursor_in_pcm_frames(&sound, &cursor);
        return cursor;
    }

    float Audio::GetDurationInSeconds() const
    {
        if (!initialized) return 0.0f;
        ma_uint64 lengthInFrames = GetDurationInFrames();
        ma_uint32 sampleRate = ma_engine_get_sample_rate(ma_sound_get_engine(&sound));
        return (float)lengthInFrames / (float)sampleRate;
    }

    float Audio::GetCurrentPositionInSeconds() const
    {
        if (!initialized) return 0.0f;
        ma_uint64 cursorInFrames = GetCurrentPositionInFrames();
        ma_uint32 sampleRate = ma_engine_get_sample_rate(ma_sound_get_engine(&sound));
        return (float)cursorInFrames / (float)sampleRate;
    }

    bool Audio::SeekToFrame(ma_uint64 frameIndex)
    {
        if (!initialized) return false;
        ma_result result = ma_sound_seek_to_pcm_frame(&sound, frameIndex);
        return result == MA_SUCCESS;
    }

    bool Audio::SeekToTime(float timeInSeconds)
    {
        if (!initialized) return false;
        ma_uint32 sampleRate = ma_engine_get_sample_rate(ma_sound_get_engine(&sound));
        ma_uint64 frameIndex = (ma_uint64)(timeInSeconds * sampleRate);
        return SeekToFrame(frameIndex);
    }
}