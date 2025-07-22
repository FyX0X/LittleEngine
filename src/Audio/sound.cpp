#include "LittleEngine/Audio/sound.h"
#include "LittleEngine/error_logger.h"



namespace LittleEngine::Audio
{

#pragma region Object Management

	void Sound::LoadFromFile(const std::string& path, ma_engine& engine, bool spatialized)
	{
		ma_result result;

		if (spatialized) 
		{
			result = ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, &m_sound);
		} 
		else
		{
			result = ma_sound_init_from_file(&engine, path.c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &m_sound);
		}

		if (result != MA_SUCCESS) {
			LogError("Sound::Initialize: could not load sound from file: " + path + " (Error Code: " + std::to_string(result) + ')');
			return;
		}
		m_isSpatialized = spatialized;
	}

	void Sound::Shutdown()
	{
		ma_sound_uninit(&m_sound);
	}

#pragma endregion

#pragma region Play Sound

	void Sound::Play()
	{
		ma_result result;
		result = ma_sound_seek_to_pcm_frame(&m_sound, 0); // Rewind
		if (result != MA_SUCCESS) {
			LogError("Sound::Play: Could not rewind the sound.  (Error Code: " + std::to_string(result) + ')');
			return;
		}
		result = ma_sound_start(&m_sound);
		if (result != MA_SUCCESS) {
			LogError("Sound::Sound(): could not play the sound.  (Error Code: " + std::to_string(result) + ')');
			return;
		}

	}

#pragma endregion

#pragma region Sound Modifiers

	void Sound::SetVolume(float volume)
	{
		ma_sound_set_volume(&m_sound, volume);
	}

	void Sound::SetPitch(float pitch)
	{
		ma_sound_set_pitch(&m_sound, pitch);
	}

#pragma endregion

#pragma region Spatialization

	void Sound::SetSpatialization(bool spatialization)
	{
		// Dynamically disable or enable spatialization post-initialization:
		ma_sound_set_spatialization_enabled(&m_sound, spatialization);
		m_isSpatialized = spatialization;
	}
	void Sound::SetLocation(float x, float y, float z)
	{
		if (!m_isSpatialized)
		{
			LogError("Sound::SetLocation: sound is not spatialized.");
			return;
		}
		ma_sound_set_position(&m_sound, x, y, z);
	}

	void Sound::SetAttenuation(bool attenuation)
	{
		if (!m_isSpatialized)
		{
			LogError("Sound::SetAttenuation: sound is not spatialized.");
			return;
		}
		if (attenuation)
		{
			ma_sound_set_attenuation_model(&m_sound, ma_attenuation_model_inverse);
			return;
		}
		ma_sound_set_attenuation_model(&m_sound, ma_attenuation_model_none);
	}

	void Sound::SetRolloff(float value)
	{
		if (!m_isSpatialized)
		{
			LogError("Sound::SetRolloff: sound is not spatialized.");
			return;
		}
		ma_sound_set_rolloff(&m_sound, value);
	}

	void Sound::SetMinDistance(float value)
	{
		if (!m_isSpatialized)
		{
			LogError("Sound::SetMinDistance: sound is not spatialized.");
			return;
		}
		ma_sound_set_min_distance(&m_sound, value);
	}

	void Sound::SetMaxDistance(float value)
	{
		if (!m_isSpatialized)
		{
			LogError("Sound::SetMaxDistance: sound is not spatialized.");
			return;
		}
		ma_sound_set_max_distance(&m_sound, value);
	}

	void Sound::SetMinGain(float value)
	{
		if (!m_isSpatialized)
		{
			LogError("Sound::SetMinGain: sound is not spatialized.");
			return;
		}
		ma_sound_set_min_gain(&m_sound, value);
	}

	void Sound::SetMaxGain(float value)
	{
		if (!m_isSpatialized)
		{
			LogError("Sound::SetMaxGain: sound is not spatialized.");
			return;
		}
		ma_sound_set_max_gain(&m_sound, value);
	}

#pragma endregion


}

