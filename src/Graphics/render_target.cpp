#include "LittleEngine/Graphics/render_target.h"
#include "LittleEngine/Utils/logger.h"


namespace LittleEngine::Graphics
{

#pragma region Initialization / lifetime management.

	bool RenderTarget::Create(int width, int height, GLenum internalFormat)
	{
		if (width < 0 || height < 0)
		{
			Utils::Logger::Error("RenderTarget::Create: size cannot be negative but was: (" + std::to_string(width) + ", " + std::to_string(height) + ")");
			return false;
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
		m_texture.CreateEmptyTexture(width, height, internalFormat);
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
		m_width = -1;
		m_height = -1;
		m_fbo = -1;
	}

#pragma endregion


	//void RenderTarget::Clear(Color color)
	//{
	//	GLint fbo = 0;
	//	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
	//	if (fbo != m_fbo)
	//	{
	//		LogError("RenderTarget::Clear rendertarget was not bound.");
	//		Bind();
	//	}		
	//	glClearColor(color.r, color.g, color.b, color.a);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	//Unbind();
	//}


	void RenderTarget::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glViewport(0, 0, m_width, m_height); // reset viewport to target size
	}

	void RenderTarget::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default framebuffer
		//glViewport(0, 0, -1, -1); // reset viewport to target size
		// you have to set the viewport manually after unbinding, because it is not reset automatically.
	}


}