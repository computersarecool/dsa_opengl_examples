// Array texture example
#include <iostream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "base_app.h"
#include "glsl_program.h"

// Random number generator
static unsigned int seed = 0x13371337;
static inline float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}


class TextureArrayExample : public Application
{
private:
	virtual void setup()
	{
		// Load shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/simple_quad.vert").fragment("/shaders/simple_quad.frag") });

		// Create and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Make a 2d array texture
		GLuint texture_array;
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture_array);

		for (GLint i{ 0 }; i < m_num_billboards; i++)
		{
			// Get image path
			std::string image_path{ m_image_base_path + std::to_string(i) + ".jpg" };

			// Load the image
			GLint width;
			GLint height;
			GLint nr_channels;
			GLubyte* data = stbi_load(image_path.c_str(), &width, &height, &nr_channels, 0);
			if (!data)
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			// Use first image to set texture storage parameters
			if (!i)
			{
				glTextureStorage3D(texture_array, 1, GL_RGB8, width, height, m_num_billboards);
			}

			glTextureSubImage3D(texture_array, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}

		// Unnecesary if there is only one
		glBindTextureUnit(0, texture_array);

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

		m_shader->use();
		for (GLuint i = 0; i < m_num_billboards; i++)
		{
			m_shader->uniform("index", static_cast<GLfloat>(i));
			m_shader->uniform("offsetX", static_cast<GLfloat>(random_float() * 2.5 - 1.0));
			m_shader->uniform("offsetY", static_cast<GLfloat>(random_float() * 2.5 - 1.0));
			m_shader->uniform("scale", static_cast<GLfloat>(random_float() * 0.5));
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	};

protected:
	GLuint m_vao;
	GLuint m_buffer;
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
