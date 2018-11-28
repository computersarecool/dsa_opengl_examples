// This loads a jpeg image and stores it in the S3TC compressed format
// It checks that it is compressed and gets the compressed size

#include <iostream>
#include <memory>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "base_app.h"
#include "glsl_program.h"

class TextureArrayExample : public Application
{
private:
	GLuint m_vao{ 0 };
	GLuint m_texture{ 0 };
	GLint m_is_image_compressed{ 0 };
	GLint m_image_compressed_size{ 0 };
	const std::string m_image_path{ "../assets/images/0.jpg" };
	const std::vector<GLfloat> m_clear_color { 0.2f, 0.0f, 0.2f, 1.0f };
	std::unique_ptr<GlslProgram> m_shader;

	void setup() override
	{
		// Set and use shader
		m_shader = std::make_unique<GlslProgram>(GlslProgram::Format().vertex("../assets/shaders/simple_quad.vert").fragment("../assets/shaders/simple_quad.frag"));
		m_shader->use();

		// Create and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Make a texture object
		glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);

		// Load the image
		GLint width;
		GLint height;
		GLint number_of_channels;

		GLubyte* data = stbi_load(m_image_path.c_str(), &width, &height, &number_of_channels, 0);
		if (!data)
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		glTextureStorage2D(m_texture, 1, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, width, height);
		glTextureSubImage2D(m_texture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
		
		glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		

		glGetTextureLevelParameteriv(m_texture, 0, GL_TEXTURE_COMPRESSED, &m_is_image_compressed);
		if (m_is_image_compressed)
		{
			std::cout << "Texture is compressed" << std::endl;
		}


		glGetTextureLevelParameteriv(m_texture, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &m_image_compressed_size);
		{
			std::cout << "Texture size is: " << m_image_compressed_size << std::endl;
		}

		glBindTextureUnit(0, m_texture);
	}

	void render(double current_time) override
	{
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	};


};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new TextureArrayExample };
	app->run();
}
