#pragma once

#include <iostream>
#include <miniaudio.h>
#include <glm/glm.hpp>

namespace LittleEngine::Audio
{

	class Sound
	{
	public:

		Sound() = default;
		~Sound() { Shutdown(); }

		// prevent copying cause it breaks ma_sound.
		Sound(const Sound&) = delete;
		Sound& operator=(const Sound&) = delete;
		Sound(Sound&&) = delete;
		Sound& operator=(Sound&&) = delete;

		
		bool LoadFromFile(const std::string& path, ma_engine& engine, bool spatialized = false);
		void Shutdown();

		void Play();
		void Stop();
		bool IsPlaying() const;

		void SetVolume(float volume);
		void SetPitch(float pitch);
		
		void SetSpatialization(bool spatialization);
		void SetLocation(glm::vec2 pos)
		{
			SetLocation(pos.x, pos.y);
		}
		void SetLocation(float x, float y, float z = 0.f);
		void SetAttenuation(bool attenuation);
		void SetRolloff(float value);
		void SetMinDistance(float value);
		void SetMaxDistance(float value);
		void SetMinGain(float value);
		void SetMaxGain(float value);

	private:

		ma_sound m_sound = {};
		bool m_isSpatialized = false;
		bool m_isInitialized = false; // whether the sound is initialized


	};

}