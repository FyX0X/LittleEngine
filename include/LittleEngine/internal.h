#pragma once


namespace LittleEngine
{

	namespace defaults
	{
		const bool pixelated = true;
		const bool mipmaps = false;
		const float viewWidth = 32.f;		// the screen shows 32 tiles / meters in horizontal axis
		const float viewHeight = 18.f;		// the screen shows 32 tiles / meters in horizontal axis
		const int QuadCount = 1000;
		const int MAX_TEXTURE_SLOTS = 16;
		const int maxFileCount = 1000;		// used to detect next free filename (for screenshot32.png) caps at 1000
	}

	namespace internal
	{
		inline bool g_initialized = false;
	}
}