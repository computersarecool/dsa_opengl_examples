// A geometry shader quad

#include "glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "shader.h"
#include "camera.h"

// Cube: First four are positions, second three are colors
static const GLfloat vertices[]{
	-0.5, -0.5, 0, 1, 1, 0, 0,
	0.5, -0.5, 0, 1, 1, 0, 0,
	0.5, 0.5, 0, 1, 1, 0, 0,

	0.5, 0.5, 0, 1, 1, 1, 1,
	-0.5, 0.5, 0, 1, 1, 1, 1,
	-0.5, -0.5, 0, 1, 1, 1, 1
};

class GeometryShaderExample : public Application
{
private:
	virtual void set_info()
	{
		Application::set_info();	
		m_info.title = "Geometry shader example";
	}

	virtual void setup()
	{
		// Create shaders
		std::string this_path = get_parent_directory();
		m_shader = Shader{ (this_path + "/shaders/quad.vert").c_str(), (this_path + "/shaders/quad.frag").c_str(), "", "", (this_path + "/shaders/quad.geom").c_str() };

		// Cube vertex attribute parameters
		const GLuint elements_per_face{ 7 };
		const GLuint position_index{ 0 };
		const GLuint color_index{ 1 };
		const GLuint position_size{ 4 };
		const GLuint color_size{ 3 };
		const GLenum position_type{ GL_FLOAT };
		const GLenum color_type{ GL_FLOAT };
		const GLboolean position_normalize{ GL_FALSE };
		const GLboolean color_normalize{ GL_FALSE };
		const GLuint position_offset_in_buffer{ 0 };
		const GLuint color_offset_in_buffer{ sizeof(GLfloat) * position_size };

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
		glEnableVertexArrayAttrib(m_vao, color_index);

		glVertexArrayAttribFormat(m_vao, position_index, position_size, position_type, position_normalize, position_offset_in_buffer);
		glVertexArrayAttribFormat(m_vao, color_index, color_size, color_type, color_normalize, color_offset_in_buffer);

		glVertexArrayAttribBinding(m_vao, position_index, binding_index);
		glVertexArrayAttribBinding(m_vao, color_index, binding_index);

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

		// Update uniforms
		m_model_matrix = glm::rotate(m_model_matrix, m_rotation_rate, m_world_up);
		
		// Set uniforms
		m_shader.use();
		m_shader.set_mat4("uModelViewMatrix", m_camera.get_view_matrix() * m_model_matrix);
		m_shader.set_mat4("uProjectionMatrix", m_camera.get_proj_matrix());
		glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
	};

	// Member variables
	Shader m_shader;
	Shader m_normal_shader;
	GLuint m_vao;
	GLuint m_vbo;
	glm::mat4 m_model_matrix{ glm::mat4{1.0f} };
	Camera m_camera{ glm::vec3{0, 0, 5} };
	const GLuint m_num_triangles{ 6 };
	const GLfloat m_normal_length{ 0.35f};
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const GLfloat m_rotation_rate{ 0.001f };
	const GLfloat m_clear_color[4] { 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
	Application* my_app = new GeometryShaderExample;
	my_app->run();
	delete my_app;
}