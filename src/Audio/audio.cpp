#include "LittleEngine/Audio/audio.h"
#include "LittleEngine/Utils/logger.h"



namespace LittleEngine::Audio
{

#pragma region Object Management

    void AudioSystem::Initialize() {
        ma_engine_config config = ma_engine_config_init();
        ma_result result = ma_engine_init(&config, &m_engine);
        if (result != MA_SUCCESS) {
            Utils::Logger::Critical("AudioSystem::Initialize failed: " + std::to_string(result));
            m_initialized = false;
            return;
        }
        m_initialized = true;

    }
    void AudioSystem::Shutdown()
    {
        if (!m_initialized) return;

		m_initialized = false;
        ma_engine_uninit(&m_engine);
    }

#pragma endregion

#pragma region Getters

    ma_engine* AudioSystem::GetEngine()
    {
        return m_initialized ? &m_engine : nullptr;
    }

#pragma endregion

#pragma region Sound Management

    bool AudioSystem::LoadSound(const std::string& path, Sound& sound, bool spatialized)
    {
        return sound.LoadFromFile(path, m_engine, spatialized);
    }

#pragma endregion

#pragma region Spatialization

    void AudioSystem::SetListenerPosition(float x, float y, float z)
    {
        ma_engine_listener_set_position(&m_engine, 0, x, y, z);
    }

#pragma endregion
}