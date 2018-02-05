// Read pixels example
// Press spacebar to take a screenshot
#include <fstream>
#include <memory>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

// Cube: First three are positions, second three are normals
const GLfloat vertices[]{
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
	virtual void set_info()
	{
		Application::set_info();
		m_info.title = "Read pixels example";
	}

	virtual void on_key(int key, int action)
	{
		Application::on_key(key, action);

		if (key == GLFW_KEY_SPACE && GLFW_PRESS != 0)
		{
			take_screen_shot();
		}
	}

	void take_screen_shot()
	{
#pragma pack (push, 1)
		struct
		{
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

		// Make row a multiple of four
		int row_size{ ((m_info.window_width * 3 + 3) & ~3) };
		int data_size{ row_size * m_info.window_height };
		GLubyte* frame_buffer_data{ new GLubyte[data_size] };
	
		GLshort origin_x{ 0 };
		GLshort origin_y{ 0 };
		glReadPixels(origin_x, origin_y, m_info.window_width, m_info.window_height, GL_BGR, GL_UNSIGNED_BYTE, frame_buffer_data);

		memset(&tga_header, 0, sizeof(tga_header));
		tga_header.image_type = 2;
		tga_header.width = static_cast<short>(m_info.window_width);
		tga_header.height = static_cast<short>(m_info.window_height);
		tga_header.bpp = 24;

		std::ofstream screenshot;
		screenshot.open("screenshot.tga", std::ios::out | std::ios::binary);
		screenshot.write(reinterpret_cast<char*>(&tga_header), sizeof(tga_header));
		screenshot.write(reinterpret_cast<char*>(frame_buffer_data), data_size);
		screenshot.close();

		delete[] frame_buffer_data;
	}

	virtual void setup()
	{
		// Create shader
        m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/cube.vert").fragment("../assets/shaders/cube.frag")});

		// Cube vertex attribute parameters
		const GLuint elements_per_face{ 6 };
		const GLuint position_index{ 0 };
		const GLuint normal_index{ 1 };
		const GLuint position_size{ 3 };
		const GLuint normal_size{ 3 };
		const GLenum position_type{ GL_FLOAT };
		const GLenum normal_type{ GL_FLOAT };
		const GLboolean position_normalize{ GL_FALSE };
		const GLboolean normal_normalize{ GL_FALSE };
		const GLuint position_offset_in_buffer{ 0 };
		const GLuint normal_offset_in_buffer{ sizeof(GLfloat) * position_size };
		
		// Cube vertex buffer attributes
		const GLuint binding_index{ 0 };
		const GLuint offset{ 0 };
		const GLuint element_stride{ sizeof(GLfloat) * elements_per_face };

		// Setup the cube VBO and its data store
		const GLuint flags{ 0 };
		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(m_vbo, sizeof(vertices), vertices, flags);

		// Setup and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Set attributes in the VAO
		glEnableVertexArrayAttrib(m_vao, position_index);
		glEnableVertexArrayAttrib(m_vao, normal_index);
		
		glVertexArrayAttribFormat(m_vao, position_index, position_size, position_type, position_normalize, position_offset_in_buffer);
		glVertexArrayAttribFormat(m_vao, normal_index, normal_size, normal_type, normal_normalize, normal_offset_in_buffer);
		
		glVertexArrayAttribBinding(m_vao, position_index, binding_index);
		glVertexArrayAttribBinding(m_vao, normal_index, binding_index);

		glVertexArrayVertexBuffer(m_vao, binding_index, m_vbo, offset, element_stride);
		// NOTE: We are leaving the VAO bound
	}

	virtual void render(double current_time)
	{
		// Set OpenGL state
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);


		// Set uniforms and draw first cube
		m_shader->use();
		glm::mat4 model_matrix{ glm::mat4{ 1.0 } };
		model_matrix = glm::rotate(model_matrix, static_cast<GLfloat>(current_time), m_world_up);
		m_shader->uniform("uModelViewMatrix", m_camera.get_view_matrix() * model_matrix);
		m_shader->uniform("uProjectionMatrix", m_camera.get_proj_matrix());
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);


		// Set uniforms and draw second cube
		glm::mat4 model_matrix2{ glm::mat4{ 1.0 } };
		model_matrix2 = glm::translate(model_matrix2, glm::vec3{ 1.25f, 2.0f, 0.0f });
		model_matrix2 = glm::rotate(model_matrix2, static_cast<GLfloat>(current_time), m_world_up);
		model_matrix2 = glm::scale(model_matrix2, glm::vec3{ 0.5f });
		m_shader->uniform("uModelViewMatrix", m_camera.get_view_matrix() * model_matrix2);
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
	};

	// Member variables
	GLuint m_vao { 0 };
	GLuint m_vbo { 0 };
	std::unique_ptr<GlslProgram> m_shader;
	Camera m_camera{ glm::vec3{0, 0, 5} };
	const GLuint m_num_vertices{ 36 };
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new ReadPixelsExample };
	app->run();
}
