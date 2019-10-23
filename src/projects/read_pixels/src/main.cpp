// This uses glReadnPixels a safer version of glReadPixels and mapped Pixel Buffer Objects to improve performance
// It saves the rendered image as a Targa (.tga) file
// Interactivity: Press spacebar to take a screenshot

#include <fstream>
#include <memory>
#include <vector>
#include <iostream>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

// Cube cube_vertices
const GLfloat cube_vertices[]{
     // Positions         // Normals
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

class ReadPixelsExample : public Application
{
private:
	GLuint m_vao{ 0 };
	GLuint m_vbo{ 0 };
	int m_data_size{ 0 };
	int m_pbo_index{ 0 };
	Camera m_camera{ glm::vec3{ 0, 0, 5} };
	std::vector<GLuint>(m_pbos) { 0, 0 };
	const GLuint m_num_vertices{ 36 };
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const std::vector<GLfloat> m_clear_color{ 0.2f, 0.0f, 0.2f, 1.0f };
	const char* screenshot_filename { "screenshot.tga" };
	const unsigned char m_tga_image_type{ 2 };
	const unsigned char m_tga_bpp{ 24 };
	std::unique_ptr<GlslProgram> m_shader;

	void set_info() override
	{
		Application::set_info();
		m_info.title = "Read pixels example";
	}

	void on_key(int key, int action) override
	{
		Application::on_key(key, action);

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
            take_screen_shot();
		}
	}

	void take_screen_shot()
	{	// This will take a screenshot of the last frame using two PBOs for improved performance
        // Bind PBO to trigger asynchronous reads and then read data
        glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbos[m_pbo_index]);

        // Because a non-zero named buffer object is bound to the GL_PIXEL_PACK_BUFFER data is not returned in the pointer passed in at the end
        glReadnPixels(0, 0, m_info.window_width, m_info.window_height, GL_BGR, GL_UNSIGNED_BYTE, m_data_size, nullptr);

        // Swap buffers
        if (!m_pbo_index)
        {
            m_pbo_index += 1;
            // Need to wait a frame or do other work to make this really improve performance
            take_screen_shot();
        }
        else
        {
            // Tightly pack members of this struct
#pragma pack (push, 1)
            struct header {
                unsigned char ident_size;// Size of following ID field
                unsigned char cmap_type; // Color map type 0 = none
                unsigned char image_type; // Image type 2 = rgb
                short cmap_start; // First entry in palette
                short cmap_size; // Number of entries in palette
                unsigned char cmap_bpp; // Number of bits per palette
                short x_origin; // X origin
                short y_origin; // Y origin
                short width; // Width in pixels
                short height; // Height in pixels
                unsigned char bpp; // Bits per pixel
                unsigned char descriptor; // Descriptor bits
            } tga_header;
#pragma pack (pop)

            // Setup TGA header
            memset(&tga_header, 0, sizeof(tga_header));
            tga_header.image_type = m_tga_image_type;
            tga_header.width = static_cast<short>(m_info.window_width);
            tga_header.height = static_cast<short>(m_info.window_height);
            tga_header.bpp = m_tga_bpp;

            std::vector<GLubyte> framebuffer_data(static_cast<unsigned long>(m_data_size));

            // Get a pointer to client memory and copy
            // Note: This should be done a few frames later to actually be asynchronous
            const int buffer_offset{ 0 };
            void *ptr = glMapNamedBufferRange(m_pbos[m_pbo_index - 1], buffer_offset, m_data_size, GL_MAP_READ_BIT);

            if (!ptr)
            {
                check_gl_error();
            }
            else
            {
                memcpy(framebuffer_data.data(), ptr, static_cast<size_t>(m_data_size));

                // Write file
                std::ofstream screenshot;
                screenshot.open(screenshot_filename, std::ios::out | std::ios::binary);
                screenshot.write(reinterpret_cast<char *>(&tga_header), sizeof(tga_header));
                screenshot.write(reinterpret_cast<char *>(framebuffer_data.data()), m_data_size);
                screenshot.close();

                glUnmapNamedBuffer(m_pbos[m_pbo_index - 1]);
                m_pbo_index = 0;
            }
        }
	}

	void setup() override
	{
        // Set m_data_size which is determined by window height
        // Multiply window width by 3 (RGB) round up and make a multiple of 4, then multiply by window height to get byte size
        m_data_size = ((m_info.window_width * 3 + 3) & ~3) *  m_info.window_height;

		// Create shader
        m_shader = std::make_unique<GlslProgram>(GlslProgram::Format().vertex("../assets/shaders/cube.vert").fragment("../assets/shaders/cube.frag"));

		// Cube vertex attribute parameters
		const GLuint elements_per_face{ 6 };

		const GLuint position_index{ 0 };
		const GLuint position_size{ 3 };
        const GLenum position_type{ GL_FLOAT };
        const GLboolean position_normalize{ GL_FALSE };
        const GLuint position_offset_in_buffer{ 0 };

        const GLuint normal_index{ 1 };
        const GLuint normal_size{ 3 };
        const GLenum normal_type{ GL_FLOAT };
        const GLboolean normal_normalize{ GL_FALSE };
        const GLuint normal_offset_in_buffer{ sizeof(GLfloat) * position_size };

		// Cube vertex buffer attributes
		const GLuint binding_index{ 0 };
		const GLuint offset{ 0 };
		const GLuint element_stride{ sizeof(GLfloat) * elements_per_face };

		// Setup the cube VBO and its data store
		const GLuint flags{ 0 };
		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(m_vbo, sizeof(cube_vertices), cube_vertices, flags);

        // Setup the PBOs
        glCreateBuffers(static_cast<GLsizei>(m_pbos.size()), m_pbos.data());
        for (auto pbo : m_pbos)
        {
            glNamedBufferStorage(pbo, m_data_size, nullptr, GL_MAP_READ_BIT);
        }

		// Setup and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Set attributes in the VAO
		glEnableVertexArrayAttrib(m_vao, position_index);
        glVertexArrayAttribFormat(m_vao, position_index, position_size, position_type, position_normalize, position_offset_in_buffer);
        glVertexArrayAttribBinding(m_vao, position_index, binding_index);

        glEnableVertexArrayAttrib(m_vao, normal_index);
        glVertexArrayAttribFormat(m_vao, normal_index, normal_size, normal_type, normal_normalize, normal_offset_in_buffer);
		glVertexArrayAttribBinding(m_vao, normal_index, binding_index);

		glVertexArrayVertexBuffer(m_vao, binding_index, m_vbo, offset, element_stride);
	}

	void render(double current_time) override
	{
		// Set OpenGL state
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Set uniforms and draw first cube
        glm::mat4 model_matrix{ glm::mat4{ 1.0 } };
        model_matrix = glm::rotate(model_matrix, static_cast<GLfloat>(current_time), m_world_up);
        m_shader->use();
		m_shader->uniform("u_model_view_matrix", m_camera.get_view_matrix() * model_matrix);
		m_shader->uniform("u_projection_matrix", m_camera.get_proj_matrix());
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);

		// Set uniforms and draw second cube
		glm::mat4 model_matrix2{ glm::mat4{ 1.0 } };
		// The numbers here just shrink and move the cube by an arbitrary amount
		model_matrix2 = glm::translate(model_matrix2, glm::vec3{ 1.25f, 2.0f, 0.0f });
		model_matrix2 = glm::rotate(model_matrix2, static_cast<GLfloat>(current_time), m_world_up);
		model_matrix2 = glm::scale(model_matrix2, glm::vec3{ 0.5f });
		m_shader->uniform("u_model_view_matrix", m_camera.get_view_matrix() * model_matrix2);
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
	};
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new ReadPixelsExample };
	app->run();
}
