#include "LittleEngine/Graphics/tilemap_renderer.h"

#include "LittleEngine/Utils/logger.h"

namespace LittleEngine::Graphics
{
#pragma region Setters

	void TilemapRenderer::SetTileSetTexture(const Texture& texture, const TextureAtlas& atlas)
	{
		m_texture = texture;
		m_atlas = atlas;
	}

	void TilemapRenderer::SetMap(const unsigned int* map, int width, int height, glm::vec2 pos)
	{
		m_map = map;
		m_width = width;
		m_height = height;
		m_pos = pos;
	}

#pragma endregion

#pragma region Drawing

	void TilemapRenderer::Draw(Renderer* renderer)
	{
		if (m_map == nullptr)
		{
			Utils::Logger::Warning("TilemapRenderer::Draw: map was not initialized. Did you forget to call SetMap() or did you delete the original data?, skipping render call.");
			return;
		}
		DrawMap(m_map, m_width, m_height, m_pos, renderer);
	}

	void TilemapRenderer::DrawMap(const unsigned int* map, int width, int height, glm::vec2 pos, Renderer* renderer)
	{

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				Rect tileRect = {
					pos.x + x * m_tileSize, pos.y + y * m_tileSize,
					m_tileSize, m_tileSize
				};

				Rect uv = m_atlas.GetUV(m_tileAtlasCoord[m_map[y * width + x]]);

				renderer->DrawRect(tileRect, m_texture, Colors::White, uv);
			}
		}
	}

#pragma endregion

}