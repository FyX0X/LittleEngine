#pragma once

#include "LittleEngine/Graphics/renderer.h"

namespace LittleEngine::Graphics
{
	

	class TilemapRenderer
	{

	public:
		void SetTileSetTexture(const Texture& texture, const TextureAtlas& atlas);
		void SetTileSetAtlasKey(const std::vector<AtlasCoord>& tileAtlasCoord) { m_tileAtlasCoord = tileAtlasCoord; }
		void SetTileSize(float size) { m_tileSize = size; }

		/**
		 * @brief Sets the tilemap data to be rendered.
		 *
		 * This function stores a reference to the provided tilemap array. The data
		 * must remain valid for the lifetime of the renderer or until replaced.
		 * 
		 * The data must be stored in row-major format (index = y * width + x)
		 * 
		 * This data is then used when calling TilemapRender::Draw()
		 *
		 * @param map Pointer to an array of tile IDs (unsigned int).
		 * @param width Number of tiles in the horizontal direction.
		 * @param height Number of tiles in the vertical direction.
		 * @param pos Bottom-Left position of the tilemap in world coordinates.
		 */
		void SetMap(const unsigned int* map, int width, int height, glm::vec2 pos);
		
		/**
		 * @brief Draws the tilemap.
		 * 
		 * This function Draws the data that was passed in to
		 * Tilemap::SetMap(). 
		 */
		void Draw(Renderer* renderer);

		/**
		 * @brief Draws the tilemap.
		 * 
		 * This function draws the data passed as argument to the screen.
		 * 
		 * The data must be stored in row-major format (index = y * width + x)
		 * 
		 * @param map Pointer to an array of tile IDs (unsigned int).
		 * @param width Number of tiles in the horizontal direction.
		 * @param height Number of tiles in the vertical direction.
		 * @param pos Bottom-Left position of the tilemap in world coordinates.
		 */
		void DrawMap(const unsigned int* map, int width, int height, glm::vec2 pos, Renderer* renderer);


	private:

		Texture m_texture;
		TextureAtlas m_atlas;
		std::vector<AtlasCoord> m_tileAtlasCoord;
		const unsigned int* m_map = nullptr;
		int m_width = 0, m_height = 0;
		glm::vec2 m_pos = {};

		float m_tileSize = 1.f;

	

	};


}