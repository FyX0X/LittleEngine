#pragma once

#include "LittleEngine/Graphics/texture.h"


#include <ft2build.h>
#include FT_FREETYPE_H

#include <unordered_map>
#include <array>

namespace LittleEngine::Graphics
{
	struct GlyphInfo {
		glm::vec2 size = {};        // width, height in pixels
		glm::vec2 bearing = {};     // offset from baseline to top-left
		glm::vec4 uv = {};			// texture coordinates in atlas
		float advance = 0.f;         // advance in pixels
	};

	
	class Font
	{
	public:


		static void Initialize();

		bool LoadFromTTF(const std::string& path, int size, bool pixelated = false);	
		bool LoadFromData(const unsigned char* data, int dataSize, float size, bool pixelated = false);

		//const stbtt_bakedchar* GetGlyphs() const { return m_chars; }
		//const void Font::GetQuad(char c, float& xpos, float& ypos, stbtt_aligned_quad& q) const;
		const Texture& GetTexture() const {	return m_texture; }
		/**
		 * Returns a pointer to the glyph info for the given unsigned char.
		 * @param c Character code (0–255).
		 * @return Pointer to glyph info or nullptr if not present.
		 */
		const GlyphInfo* GetGlyph(unsigned char c) const;

		/**
		 * Convenience overload for signed char.
		 * @param c Character code (-128:127).
		 * @return Pointer to glyph info or nullptr if not present.
		 */
		inline const GlyphInfo* GetGlyph(char c) const
		{
			return GetGlyph(static_cast<unsigned char>(c));
		}

		const std::array<GlyphInfo, 256>& GetGlyphs() const { return m_glyphs; }
		const std::array<bool, 256>& GetGlyphsPresence() const { return m_isCharPresent; }

		static Font GetDefaultFont(float size);

		void Bind() const { m_texture.Bind(); }
		void Unbind() const { m_texture.Unbind(); }
		float GetSize() const { return m_size; }

	private:

		void GenerateAtlas(bool pixelated = false);

		static FT_Library s_ft;
		static bool s_initialized;
		FT_Face m_face;
		Texture m_texture;
		// Pack glyphs into atlas and store their info
		// 256 even though only 95 or less are used.
		std::array<bool, 256> m_isCharPresent{};
		std::array<GlyphInfo, 256> m_glyphs{};
		float m_size = 0;


	};


}