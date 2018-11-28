// This is a pass through geometry shader

#include <memory>
#include <vector>

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

static const GLfloat vertices[]{
	-0.5f, -0.5f, 0.0f, 1.0f,
	 0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f,  0.5,  0.0f, 1.0f,
	 0.5f,  0.5,  0.0f, 1.0f
};

class GeometryShaderExample : public Application
{
private:

	bool m_show_wireframe{ false };
	const GLuint m_vertices_per_patch{ 4 };
	const std::vector<GLfloat> m_clear_color{ 0.2f, 0.0f, 0.2f, 1.0f };
	glm::mat4 m_model_matrix{ glm::mat4{ 1.0f } };
	glm::mat4 m_view_matrix{ 1.0 };
	glm::mat4 m_projection_matrix{ 1.0 };
	GLuint m_vao{ 0 };
	GLuint m_vbo{ 0 };
	Camera m_camera;
	std::unique_ptr<GlslProgram> m_shader;

	void set_info() override
	{
		Application::set_info();	
		m_info.title = "Geometry shader example";
	}

	void on_key(int key, int action) override
	{
		Application::on_key(key, action);
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			m_show_wireframe = !m_show_wireframe;
		}
	}

	void setup() override
	{
		// Create and enable shader
		m_shader = std::make_unique<GlslProgram>(GlslProgram::Format().vertex("../assets/shaders/shader.vert").fragment("../assets/shaders/shader.frag").geometry("../assets/shaders/shader.geom"));
		m_shader->use();

		// Vertex attribute parameters
		const GLuint position_index{ 0 };
		const GLuint size{ 4 };
		const GLenum type{ GL_FLOAT };
		const GLboolean normalized{ GL_FALSE };
		const GLuint stride{ sizeof(GLfloat) * size };

		// Set up VBO
		const GLuint flags{ 0 };
		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(m_vbo, sizeof(vertices), vertices, flags);

		// Buffer attributes
		const GLuint relative_offset{ 0 };
		const GLuint binding_index{ 0 };
		const GLuint offset{ 0 };

		// Setup and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glEnableVertexArrayAttrib(m_vao, position_index);
		glVertexArrayAttribFormat(m_vao, position_index, size, type, normalized, relative_offset);
		glVertexArrayVertexBuffer(m_vao, binding_index, m_vbo, offset, stride);
		glVertexArrayAttribBinding(m_vao, position_index, binding_index);
	}

	void render(double current_time) override
	{
		// Set OpenGL state
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glPatchParameteri(GL_PATCH_VERTICES, m_vertices_per_patch);

		// Show or hide wireframe
		if (m_show_wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Set uniforms
		m_view_matrix = m_camera.get_view_matrix();
		m_projection_matrix = m_camera.get_proj_matrix();
		m_shader->uniform("u_model_view_matrix", m_view_matrix * m_model_matrix);
		m_shader->uniform("u_projection_matrix", m_projection_matrix * m_model_matrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	};
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new GeometryShaderExample};
	app->run();
}