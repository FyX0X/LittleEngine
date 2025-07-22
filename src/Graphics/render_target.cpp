#include "LittleEngine/Graphics/render_target.h"
#include "LittleEngine/error_logger.h"


namespace LittleEngine::Graphics
{

#pragma region Initialization / lifetime management.

	bool RenderTarget::Create(int width, int height)
	{
		if (width < 0 || height < 0)
		{
			ThrowError("RenderTarget::Create: size cannot be negative but was: (" + std::to_string(width) + ", " + std::to_string(height) + ")");
		}
		m_width = width;
		m_height = height;

		glGenFramebuffers(1, &m_fbo);
		Bind();

		/*	Conditions:
		We have to attach at least one buffer (color, depth or stencil buffer).
		There should be at least one color attachment.
		All attachments should be complete as well (reserved memory).
		Each buffer should have the same number of samples.
		*/

		// create empty texture
		m_texture.CreateEmptyTexture(width, height, 3);
		// bind the texture to the frame buffer.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.id, 0);

		// attach depth, stencil buffer ? TODO

		bool success = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;		// true if success

		Unbind();

		return success;
	}

	void RenderTarget::Cleanup()
	{
		glDeleteFramebuffers(1, &m_fbo);
		m_texture.Cleanup();
	}

#pragma endregion



}