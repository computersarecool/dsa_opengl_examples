// This loads an image in the S3TC compressed format
// It checks to make sure it is compressed and gets the compressed size

#include <iostream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "base_app.h"
#include "glsl_program.h"

class TextureArrayExample : public Application
{
private:
	virtual void setup() override
	{
		// Set and use shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/simple_quad.vert").fragment("../assets/shaders/simple_quad.frag") });
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
		
		GLint is_compressed{ 0 };
		glGetTextureLevelParameteriv(m_texture, 0, GL_TEXTURE_COMPRESSED, &is_compressed);
		if (is_compressed)
		{
			std::cout << "Texture is compressed" << std::endl;
		}

		GLint compressed_size{ 0 };
		glGetTextureLevelParameteriv(m_texture, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size);
		{
			std::cout << "Texture size is: " << compressed_size << std::endl;
		}

		glBindTextureUnit(0, m_texture);
	}

	virtual void render(double current_time) override
	{
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	};

protected:
	GLuint m_vao { 0 };
	GLuint m_texture { 0 };
	const std::string m_image_path{ "../assets/images/0.jpg" };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
	std::unique_ptr<GlslProgram> m_shader;
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new TextureArrayExample };
	app->run();
}
