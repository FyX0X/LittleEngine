#pragma once

#include <miniaudio.h>
#include <glm/glm.hpp>
#include "LittleEngine/Audio/sound.h"



namespace LittleEngine::Audio
{

	class AudioSystem
	{
	public:
		AudioSystem() = default;
		~AudioSystem() { Shutdown(); }

		// prevent copy because ma_engine is not movable (internal state pointers)
		AudioSystem(const AudioSystem&) = delete;
		AudioSystem& operator=(const AudioSystem&) = delete;
		AudioSystem(AudioSystem&&) = delete;
		AudioSystem& operator=(AudioSystem&&) = delete;
		
		void Initialize();
		void Shutdown();

		ma_engine* GetEngine();
		bool IsInitialized() const { return m_initialized; }

		void LoadSound(const std::string& path, Sound& sound, bool spatialized = false);

		void SetListenerPosition(glm::vec2 pos)
		{
			SetListenerPosition(pos.x, pos.y);
		}
		void SetListenerPosition(float x, float y, float z = 0.f);


	private:
		ma_engine m_engine;
		bool m_initialized = false;


	};



}