#include <iostream>

#include "base_app.h"
#include "shader.h"

// Three triangles
static const GLfloat vertices[]{
	-0.75, -1.0, 0,
	0, -1.0, 0,
	-0.75, 0, 0,

	0.0, -1.0, 0,
	0.75, -1.0, 0,
	0.0, 0, 0,

	-0.25, 0, 0,
	0.5, 0, 0,
	-0.25, 1, 0
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
		std::string this_path = get_parent_directory();
		m_shader_one = Shader{ (this_path + "/shaders/shader.vert").c_str(), (this_path + "/shaders/shader.frag").c_str(), (this_path + "/shaders/shader.tesc").c_str(),  (this_path + "/shaders/shader.tese").c_str() };
		m_shader_two = Shader{ (this_path + "/shaders/shader.vert").c_str(), (this_path + "/shaders/shader.frag").c_str(),  (this_path + "/shaders/shader.tesc").c_str(),  (this_path + "/shaders/shader.tese").c_str() };
		m_shader_three = Shader{ (this_path + "/shaders/shader.vert").c_str(), (this_path + "/shaders/shader.frag").c_str(),  (this_path + "/shaders/shader.tesc").c_str(), (this_path + "/shaders/shader.tese").c_str() };

		// Vertex attribute parameters
		const GLuint attrib_index{ 0 };
		const GLuint size{ 3 };
		const GLenum type{ GL_FLOAT };
		const GLboolean normalized{ GL_FALSE };
		const GLuint stride{ sizeof(GLfloat) * 3 };
		const GLuint flags{ 0 };

		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(m_vbo, sizeof(vertices), vertices, flags);

		const GLuint relative_offset{ 0 };
		const GLuint binding_index{ 0 };
		const GLuint offset{ 0 };

		glCreateVertexArrays(1, &m_vao);
		glEnableVertexArrayAttrib(m_vao, attrib_index);
		glVertexArrayAttribFormat(m_vao, attrib_index, size, type, normalized, relative_offset);
		glVertexArrayVertexBuffer(m_vao, binding_index, m_vbo, offset, stride);
		glVertexArrayAttribBinding(m_vao, attrib_index, binding_index);

		// Specify vertices per patch (3 is default)
		glPatchParameteri(GL_PATCH_VERTICES, m_vertices_per_patch);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	virtual void render(double current_time)
	{
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		m_shader_one.use();
		glBindVertexArray(m_vao);
		m_shader_one.set_float("tessLevel", m_tess_level);
		glDrawArrays(GL_PATCHES, 0, m_vertices_per_face);

		m_shader_two.use();
		glBindVertexArray(m_vao);
		m_shader_two.set_float("tessLevel", m_tess_level);
		glDrawArrays(GL_PATCHES, 3, m_vertices_per_face);

		m_shader_three.use();
		glBindVertexArray(m_vao);
		m_shader_three.set_float("tessLevel", m_tess_level);
		glDrawArrays(GL_PATCHES, 6, m_vertices_per_face);
	};

	Shader m_shader_one;
	Shader m_shader_two;
	Shader m_shader_three;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_vertices_per_face{ 3 };
	GLuint m_vertices_per_patch{ 3 };
	GLfloat m_min_tess_level{ 1.0f };
	GLfloat m_tess_level{ m_min_tess_level };
	GLfloat m_tess_increment{ 0.05f };
	GLint m_max_tess_level;
	GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
	Application* my_app = new TesselationExample;
	my_app->run();
	delete my_app;
}
