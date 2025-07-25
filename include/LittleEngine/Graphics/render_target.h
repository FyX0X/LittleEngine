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

		void Bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); }
		void Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		void Clear(Color color = Colors::ClearColor);
		
		const Texture& GetTexture() { return m_texture; }

		glm::ivec2 GetSize() { return glm::ivec2(m_width, m_height); }

	private:
		Texture m_texture;
		GLuint m_fbo;
		int m_width = -1;
		int m_height = -1;
		// GLuint m_depthBuffer; TODO add later maybe.


	};


}