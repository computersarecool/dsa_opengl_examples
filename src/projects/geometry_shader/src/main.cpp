// A geometry shader example
#include <memory>

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
	virtual void set_info()
	{
		Application::set_info();	
		m_info.title = "Geometry shader example";
	}

	virtual void on_key(int key, int action)
	{
		Application::on_key(key, action);
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			m_show_wireframe = !m_show_wireframe;
		}
	}

	virtual void setup()
	{
		// Create shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/shader.vert").fragment("../assets/shaders/shader.frag").geometry("../assets/shaders/shader.geom") });
		m_shader->use();

		// Vertex attribute parameters
		const GLuint position_index{ 0 };
		const GLuint size{ 4 };
		const GLenum type{ GL_FLOAT };
		const GLboolean normalized{ GL_FALSE };
		const GLuint stride{ sizeof(GLfloat) * size };

		// Set up buffer storage
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

	virtual void render(double current_time)
	{
		// Set OpenGL state
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
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
		m_shader->uniform("uModelViewMatrix", m_view_matrix * m_model_matrix);
		m_shader->uniform("uProjectionMatrix", m_projection_matrix * m_model_matrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	};

	// Member variables
	GLuint m_vao { 0 };
	GLuint m_vbo { 0 };
	bool m_show_wireframe{ false };
	GLuint m_vertices_per_patch{ 4 };
	GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
	Camera m_camera;
	glm::mat4 m_model_matrix{ glm::mat4{ 1.0f } };
	glm::mat4 m_view_matrix;
	glm::mat4 m_projection_matrix;
	std::unique_ptr<GlslProgram> m_shader;
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new GeometryShaderExample};
	app->run();
}