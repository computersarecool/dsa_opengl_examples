// A basic cube example
#include "glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "shader.h"
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

class BasicCubeExample : public Application
{
private:
	virtual void set_info()
	{
		Application::set_info();	
		m_info.title = "Basic cube example";
	}

	virtual void setup()
	{
		// Create shader
		std::string this_path = get_parent_directory();
		m_shader = Shader{ (this_path + "/shaders/cube.vert").c_str(), (this_path + "/shaders/cube.frag").c_str() };

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
		m_shader.use();
		glm::mat4 model_matrix{ glm::mat4{ 1.0 } };
		model_matrix = glm::rotate(model_matrix, static_cast<GLfloat>(current_time), m_world_up);
		m_shader.set_mat4("uModelViewMatrix", m_camera.get_view_matrix() * model_matrix);
		m_shader.set_mat4("uProjectionMatrix", m_camera.get_proj_matrix());
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);


		// Set uniforms and draw second cube
		glm::mat4 model_matrix2{ glm::mat4{ 1.0 } };
		model_matrix2 = glm::translate(model_matrix2, glm::vec3{ 1.25f, 2.0f, 0.0f });
		model_matrix2 = glm::rotate(model_matrix2, static_cast<GLfloat>(current_time), m_world_up);
		model_matrix2 = glm::scale(model_matrix2, glm::vec3{ 0.5f });
		m_shader.set_mat4("uModelViewMatrix", m_camera.get_view_matrix() * model_matrix2);
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
	};

	// Member variables
	Shader m_shader;
	GLuint m_vao;
	GLuint m_vbo;
	Camera m_camera{ glm::vec3{0, 0, 5} };
	const GLuint m_num_vertices{ 36 };
	const GLfloat m_normal_length{ 0.5f};
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
        Application* my_app = new BasicCubeExample;
	my_app->run();
	delete my_app;
}
