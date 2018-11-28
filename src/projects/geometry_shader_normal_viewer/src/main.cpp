// This is a cube geometry shader which converts faces to lines to show normals

#include <memory>
#include <vector>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

// Cube
const GLfloat cube_vertices[] {
	 // Positions	      // Normals
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


     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f
};

class GeometryShaderExample : public Application
{
private:
	GLuint m_vao{ 0 };
	GLuint m_vbo{ 0 };
	glm::mat4 m_model_matrix{ glm::mat4{1.0f } };
	Camera m_camera{ glm::vec3{0, 0, 5} };
	const GLuint m_num_vertices{ 36 };
	const GLfloat m_normal_length{ 0.5f};
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const GLfloat m_rotation_rate{ 0.001f };
	const std::vector<GLfloat> m_clear_color{ 0.2f, 0.0f, 0.2f, 1.0f };
	std::unique_ptr<GlslProgram> m_shader;
	std::unique_ptr<GlslProgram> m_normal_shader;

	void set_info() override
	{
		Application::set_info();
		m_info.title = "Geometry shader example";
	}

	void setup() override
	{
		// Create shaders
		m_shader = std::make_unique<GlslProgram>(GlslProgram::Format().vertex("../assets/shaders/cube.vert").fragment("../assets/shaders/cube.frag"));
		m_normal_shader = std::make_unique<GlslProgram>(GlslProgram::Format().vertex("../assets/shaders/normal_viewer.vert").fragment("../assets/shaders/normal_viewer.frag").geometry("../assets/shaders/normal_viewer.geom"));

		// Cube position vertex attribute parameters
        const GLuint elements_per_face{ 6 };
		const GLuint position_index{ 0 };
		const GLuint position_size{ 3 };
        const GLenum position_type{ GL_FLOAT };
        const GLboolean position_normalize{ GL_FALSE };
        const GLuint position_offset_in_buffer{ 0 };

        // Normal position vertex attribute parameters
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

		// Turn on depth test
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void render(double current_time) override
	{
		// Set OpenGL state
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		// Update uniforms
		m_model_matrix = glm::rotate(m_model_matrix, m_rotation_rate, m_world_up);

		// Set uniforms for normals
		m_normal_shader->use();
		m_normal_shader->uniform("u_model_view_matrix", m_camera.get_view_matrix() * m_model_matrix);
		m_normal_shader->uniform("u_projection_matrix", m_camera.get_proj_matrix());
		m_normal_shader->uniform("u_normal_length", m_normal_length);
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);

        // Set uniforms for faces
		m_shader->use();
		m_shader->uniform("u_model_view_matrix", m_camera.get_view_matrix() * m_model_matrix);
		m_shader->uniform("u_projection_matrix", m_camera.get_proj_matrix());
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
	};
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new GeometryShaderExample };
	app->run();
}