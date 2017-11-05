// Array texture example
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "base_app.h"
#include "shader.h"

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
		std::string this_path = get_parent_directory();
		m_shader = Shader{ (this_path + "/shaders/simple_quad.vert").c_str(), (this_path + "/shaders/simple_quad.frag").c_str() };

		// Create and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Make a 2d array texture
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);

		for (GLint i = 0; i < m_num_squares; i++)
		{
			// Get image path
			std::string base_path = (this_path + "/assets/array/");
			base_path += std::to_string(i);
			base_path += ".jpg";

			// Load the image
			GLint width;
			GLint height;
			GLint nr_channels;
			GLubyte* data = stbi_load(base_path.c_str(), &width, &height, &nr_channels, 0);
			if (!data)
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			// Use first image to set texture storage parameters
			if (!i)
			{
				glTextureStorage3D(texture, 1, GL_RGB8, width, height, m_num_squares);
			}
			
			glTextureSubImage3D(texture, 0, 0, 0, i, width, height, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}

		// Unnecesary if there is only one
		glBindTextureUnit(0, texture);

		// Other OpenGL settings
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	virtual void render(double current_time)
	{
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		m_shader.use();
		for (GLuint i = 0; i < m_num_squares; i++)
		{
			m_shader.set_float("index", static_cast<GLfloat>(i));
			m_shader.set_float("offsetX", random_float() * 2.5 - 1.0);
			m_shader.set_float("offsetY", random_float() * 2.5 - 1.0);
			m_shader.set_float("scale", random_float() * 0.5);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	};

protected:
	Shader m_shader;
	GLuint m_vao;
	GLuint m_buffer;
	const GLuint m_num_squares{ 36 };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
	Application* my_app = new TextureArrayExample;
	my_app->run();
	delete my_app;
	return 0;
}
