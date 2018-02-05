// Array texture example
#include <iostream>
#include <memory>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "base_app.h"
#include "glsl_program.h"

class TextureArrayExample : public Application
{
private:
	virtual void setup()
	{
		// Set and use shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/simple_quad.vert").fragment("../assets/shaders/simple_quad.frag") });
		m_shader->use();

		// Create and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Make a 2D array texture
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_texture_array);

		for (int i{ 0 }; i < m_num_billboards; i++)
		{
			// Get image path
			std::string image_path{ m_image_base_path + std::to_string(i) + ".jpg" };

			// Load the image
			GLint width;
			GLint height;
			GLint number_of_channels;
			GLubyte* data = stbi_load(image_path.c_str(), &width, &height, &number_of_channels, 0);
			if (!data)
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			// Use first image to set texture storage parameters
			if (!i)
			{
				glTextureStorage3D(m_texture_array, 1, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, width, height, m_num_billboards);
			}

			glTextureSubImage3D(m_texture_array, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		
		glTextureParameteri(m_texture_array, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_texture_array, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// This is also the default
		glBindTextureUnit(0, m_texture_array);

		// Set OpenGL State
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	virtual void render(double current_time)
	{
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		for (int i{ 0 }; i < m_num_billboards; i++)
		{
			// These require narrowing
			const GLuint index = static_cast<GLuint>(i);
			const GLfloat offset_x = static_cast<GLfloat>(std::fmod(current_time / 2, 3) * 2.0 - 3.0);
			const GLfloat offset_y = static_cast<GLfloat>(6 - index * 2.0);
			const GLfloat scale = static_cast<GLfloat>(0.1 + index / 8);
			m_shader->uniform("imageIndex", index);
			m_shader->uniform("offsetX", offset_x);
			m_shader->uniform("offsetY", offset_y);
			m_shader->uniform("scale", scale);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	};

protected:
	GLuint m_vao { 0 };
	GLuint m_texture_array { 0 };
	std::unique_ptr<GlslProgram> m_shader;
	const std::string m_image_base_path{ "../assets/texture_array/" };
	const GLuint m_num_billboards{ 4 };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new TextureArrayExample };
	app->run();
}
