#pragma once

#include <glad/glad.h>

#include "LittleEngine/Graphics/texture.h"
#include "LittleEngine/Graphics/color.h"


namespace LittleEngine::Graphics
{


	class RenderTarget
	{
	public:

		bool Create(int width, int height, GLenum internalFormat = GL_RGB);			// later: bool hasDepthBuffer ??
		void Cleanup();

		void Bind();
		void Unbind();

		//void Clear(Color color = Colors::Black);
		
		const Texture& GetTexture() { return m_texture; }

		glm::ivec2 GetSize() { return glm::ivec2(m_width, m_height); }

	private:
		Texture m_texture = {};
		GLuint m_fbo = -1;
		int m_width = -1;
		int m_height = -1;
		// GLuint m_depthBuffer; TODO add later maybe.


	};


}