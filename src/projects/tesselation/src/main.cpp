#include <iostream>
#include <memory>

#include "base_app.h"
#include "glsl_program.h"

// Positions of three triangles
const GLfloat vertices[]{
	-0.75f, -1.0f, 0.0f,
	 0.0f,  -1.0f, 0.0f,
	-0.75f,  0.0f, 0.0f,

	 0.0f,  -1.0f, 0.0f,
	 0.75f, -1.0f, 0.0f,
	 0.0f,   0.0f, 0.0f,

	-0.25f,  0.0f, 0.0f,
	 0.5f, 	 0.0f, 0.0f,
	-0.25f,  1.0f, 0.0f
};

class TesselationExample : public Application
{
private:
	virtual void on_key(int key, int action)
	{
		Application::on_key(key, action);

		if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			m_tess_level < m_max_tess_level ? m_tess_level += m_tess_increment : m_max_tess_level;
		}
		else if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			m_tess_level > m_min_tess_level ? m_tess_level -= m_tess_increment : m_tess_level;
		}
	}

	virtual void set_info()
	{
		Application::set_info();
		m_info.title = "Tesselation Example";
	}

	virtual void setup()
	{
		glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &m_max_tess_level);

		// Create shaders
		m_shader_one.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/shader.vert").fragment("../assets/shaders/shader.frag").tess_control("../assets/shaders/shader.tesc").tess_eval("../assets/shaders/shader.tese") });
		m_shader_two.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/shader.vert").fragment("../assets/shaders/shader.frag").tess_control("../assets/shaders/shader.tesc").tess_eval("../assets/shaders/shader2.tese") });
		m_shader_three.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/shader.vert").fragment("../assets/shaders/shader.frag").tess_control("../assets/shaders/shader.tesc").tess_eval("../assets/shaders/shader3.tese") });

		// Vertex attribute parameters
		const GLuint position_index{ 0 };
		const GLuint position_size{ 3 };
		const GLenum position_type{ GL_FLOAT };
		const GLboolean position_normalize{ GL_FALSE };
        const GLuint position_offset_in_buffer{ 0 };

        // VBO attributes
        const GLuint binding_index{ 0 };
        const GLuint first_element_offset{ 0 };
        const GLuint element_stride{ sizeof(GLfloat) * 3 };

        // Setup triangle VBO
        const GLuint flags{ 0 };
		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(m_vbo, sizeof(vertices), vertices, flags);

        // Set up position attribute in VAO
		glCreateVertexArrays(1, &m_vao);
		glEnableVertexArrayAttrib(m_vao, position_index);
		glVertexArrayAttribFormat(m_vao, position_index, position_size, position_type, position_normalize, position_offset_in_buffer);
		glVertexArrayVertexBuffer(m_vao, binding_index, m_vbo, first_element_offset, element_stride);
		glVertexArrayAttribBinding(m_vao, position_index, binding_index);

		// Specify vertices per patch
		glPatchParameteri(GL_PATCH_VERTICES, m_vertices_per_patch);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	virtual void render(double current_time)
	{
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		m_shader_one->use();
		glBindVertexArray(m_vao);
		m_shader_one->uniform("tessLevel", m_tess_level);
		glDrawArrays(GL_PATCHES, 0, m_vertices_per_face);

		m_shader_two->use();
		glBindVertexArray(m_vao);
		m_shader_two->uniform("tessLevel", m_tess_level);
		glDrawArrays(GL_PATCHES, 3, m_vertices_per_face);

		m_shader_three->use();
		glBindVertexArray(m_vao);
		m_shader_three->uniform("tessLevel", m_tess_level);
		glDrawArrays(GL_PATCHES, 6, m_vertices_per_face);
	};

	GLuint m_vao { 0 };
	GLuint m_vbo { 0 };;
	GLuint m_vertices_per_face{ 3 };
	GLuint m_vertices_per_patch{ 3 };
	GLfloat m_min_tess_level{ 1.0f };
	GLfloat m_tess_level{ m_min_tess_level };
	GLfloat m_tess_increment{ 0.05f };
	GLint m_max_tess_level { 0 };
	GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
    std::unique_ptr<GlslProgram> m_shader_one;
    std::unique_ptr<GlslProgram> m_shader_two;
    std::unique_ptr<GlslProgram> m_shader_three;
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new TesselationExample };
	app->run();
}
