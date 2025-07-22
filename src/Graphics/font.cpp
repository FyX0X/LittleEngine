
#include "LittleEngine/Graphics/font.h"
#include "LittleEngine/Graphics/default_font.h"
#include "LittleEngine/error_logger.h"


#undef max


namespace LittleEngine::Graphics
{

	bool Font::s_initialized = false;
	FT_Library Font::s_ft = 0;

#pragma region Intitialization / loading

	void Font::Initialize()
	{
		if (!s_initialized)
		{
			s_initialized = true;
			if (FT_Init_FreeType(&s_ft))
			{
				s_initialized = false;
				LogError("Font::Initialize: Could not init FreeType Library");
			}

		}


	}

	void Font::LoadFromTTF(const std::string& path, int size, bool pixelated)
	{
		if (FT_New_Face(s_ft, path.c_str(), 0, &m_face))
		{
			LogError("Font::LoadFromTTF: Failed to load font: " + path);
		}
		FT_Set_Pixel_Sizes(m_face, 0, size);

		m_size = size;

		GenerateAtlas(pixelated);
	}

	void Font::LoadFromData(const unsigned char* data, int dataSize, float size, bool pixelated)
	{


		if (FT_New_Memory_Face(s_ft,			// Your FT_Library
							   data,			// Pointer to the .ttf data in memory
							   dataSize,		// Size of the font data in bytes
							   0,				// Face index (usually 0 unless font has multiple faces)
							   &m_face)
			)
		{
			LogError("Font::LoadFromTTF: Failed to load font");
		}
		FT_Set_Pixel_Sizes(m_face, 0, size);

		m_size = size;

		GenerateAtlas(pixelated);

	}

#pragma endregion

#pragma region Getters

	const GlyphInfo* Font::GetGlyph(unsigned char c) const
	{
		if (m_isCharPresent[c])
			return &m_glyphs[c];
		return nullptr;
	}

	Font Font::GetDefaultFont(float size)
	{
		Font def;
		def.LoadFromData(defaultFontTTF, sizeof(defaultFontTTF), size, true);
		return def;
	}

#pragma endregion

#pragma region Helper

	void Font::GenerateAtlas(bool pixelated)
	{

		int atlasWidth = 512;
		int atlasHeight = 512;
		unsigned char* atlasPixels = new unsigned char[atlasWidth * atlasHeight];
		memset(atlasPixels, 0, atlasWidth * atlasHeight);


		int x = 0, y = 0, rowHeight = 0;


		for (char c = 32; c < 127; ++c) {
			if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) continue;

			auto& bmp = m_face->glyph->bitmap;

			// line wrap
			if (x + bmp.width >= atlasWidth) {
				x = 0;
				y += rowHeight + 1;
				rowHeight = 0;
			}

			// Copy glyph to atlas
			for (int j = 0; j < bmp.rows; ++j)
				for (int i = 0; i < bmp.width; ++i)
					atlasPixels[(x + i) + (y + j) * atlasWidth] = bmp.buffer[i + j * bmp.pitch];

			// Save glyph metrics
			GlyphInfo info;
			info.size = { bmp.width, bmp.rows };
			info.bearing = { m_face->glyph->bitmap_left, m_face->glyph->bitmap_top };
			info.advance = m_face->glyph->advance.x / 64.0f;
			info.uv = {
				x / (float)atlasWidth, (y + bmp.rows) / (float)atlasHeight,
				(x + bmp.width) / (float)atlasWidth, y / (float)atlasHeight
			};
			/*	ONLY IF DECIDE TO REVERSE FONT BITMAP
			info.uv = {
				x / (float)atlasWidth, 1.f - (y + bmp.rows) / (float)atlasHeight,
				(x + bmp.width) / (float)atlasWidth, 1.f - y / (float)atlasHeight
			};*/

			m_glyphs[c] = info;
			m_isCharPresent[c] = true;


			x += bmp.width + 1;
			rowHeight = std::max(rowHeight, (int)bmp.rows);



		}


		m_texture.LoadFromData(atlasPixels, atlasWidth, atlasHeight, 1, pixelated, false, false);

		delete[] atlasPixels;
		FT_Done_Face(m_face);
	}

#pragma endregion

}