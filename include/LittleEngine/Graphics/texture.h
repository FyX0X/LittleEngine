#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "LittleEngine/internal.h"
#include <string>

namespace LittleEngine::Graphics
{
#pragma region Texture

	class Texture
	{
	public:
		GLuint id = 0;
		int width = -1;
		int height = -1;

		void CreateEmptyTexture(int width, int height, int channelCount);
		void LoadFromFile(const std::string& path, bool pixelated = defaults::pixelated, bool mipmaps = defaults::mipmaps, bool verticalFlip = false);
		void LoadFromData(const unsigned char* data, int width, int height, int channelCount = 4, bool pixelated = defaults::pixelated, bool mipmaps = defaults::mipmaps, bool verticalFlip = false);
		//glm::ivec2 GetSize();


		void Bind(const unsigned int sample = 0) const;
		void Unbind() const;

		void Cleanup();

		static Texture GetDefaultTexture();
	private:
		static void CreateDefaultTexture();
		static GLuint s_defaultTexId;
	};
#pragma endregion



#pragma region TextureAtlas
	struct AtlasCoord
	{
		int x;
		int y;
	};

	struct TextureAtlas
	{
		TextureAtlas() {};
		TextureAtlas(Texture texture, int cellWidth, int cellHeight);

		int textureWidth = 0;
		int textureHeight = 0;
		int cellWidth = 0;
		int cellHeight = 0;

		// Returns UV coords of block in (u0, v0, u1, v1) format.
		glm::vec4 GetUV(AtlasCoord coord)
		{
			return GetUV(coord.x, coord.y);
		}
		glm::vec4 GetUV(int x, int y);


	};



}