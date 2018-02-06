// Multiple attributes and multiple buffers example
// https://stackoverflow.com/a/21652955/3291506 explains buffer binding indices
#include <memory>

#include "base_app.h"
#include "glsl_program.h"

static const GLfloat vertices[]{
	// Positions          // Colors
	-0.5f, -0.5f, 0.0f,  1.0, 0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,  0.0f, 1.0, 0.0f,
	-0.5f,  0.5,  0.0f,	 0.0f, 0.0f, 1.0f,

	-0.5f,  0.5,  0.0f,  0.0f, 0.0f, 1.0f,
	 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
	 0.5f,  0.5,  0.0f,  1.0f, 0.0f, 1.0f
};

static const GLfloat vertices2[]{
	// Positions      // Colors
	-0.5f, -0.5f, 0.0f,  1.0, 0.0f, 0.0f,
	 0.5, -0.5f,  0.0f,  0.0f, 1.0, 0.0f,
	-0.5f, 0.5f,  0.0f,	 0.0f, 0.0f, 1.0f,

	-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 0.0f,
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 1.0f,
	 0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f
};

class MultipleAttributeExample : public Application
{
private:
	// Pressing spacebar toggles which buffer backs the VAO
	virtual void on_key(int key, int action)
	{
		Application::on_key(key, action);

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			m_buffer_zero = !m_buffer_zero;

			if (m_buffer_zero)
				glVertexArrayVertexBuffer(m_vao, m_attrib_binding_index, m_buffers[0], m_attrib_offset, m_attrib_stride);

			else
				glVertexArrayVertexBuffer(m_vao, m_attrib_binding_index, m_buffers[1], m_attrib_offset, m_attrib_stride);
		}
	}

	virtual void setup()
	{
		// Create shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/shader.vert").fragment("../assets/shaders/shader.frag")});

		// Create buffers
		const GLuint flags{ 0 };
		const GLuint buffer_offset{ 0 };
		const GLsizeiptr buffer_size{ sizeof(vertices) };

		glCreateBuffers(2, m_buffers);
		glNamedBufferStorage(m_buffers[0], buffer_size, nullptr, GL_MAP_WRITE_BIT);
		glNamedBufferStorage(m_buffers[1], buffer_size, nullptr, GL_MAP_WRITE_BIT);
		
		// Map buffers
		void* ptr = glMapNamedBufferRange(m_buffers[0], buffer_offset, buffer_size, GL_MAP_WRITE_BIT);
		memcpy(ptr, vertices, static_cast<size_t>(buffer_size));
		glUnmapNamedBuffer(m_buffers[0]);

		void* ptr2 = glMapNamedBufferRange(m_buffers[1], buffer_offset, buffer_size, GL_MAP_WRITE_BIT);
		memcpy(ptr2, vertices2, static_cast<size_t>(buffer_size));
		glUnmapNamedBuffer(m_buffers[1]);

		//  Attribute parameters
		const GLboolean normalize_attrib{ GL_FALSE };

		// Positions
		const GLuint position_index{ 0 };
		const GLuint position_size{ 3 };
		const GLenum position_type{ GL_FLOAT };
		const GLuint position_offset{ 0 };

		// Colors
		const GLuint color_index{ 1 };
		const GLuint color_size{ 3 };
		const GLenum color_type{ GL_FLOAT };
		const GLuint color_offset{ 12 };

		glCreateVertexArrays(1, &m_vao);
		glEnableVertexArrayAttrib(m_vao, position_index);
		glEnableVertexArrayAttrib(m_vao, color_index);
		glVertexArrayAttribFormat(m_vao, position_index, position_size, position_type, normalize_attrib, position_offset);
		glVertexArrayAttribFormat(m_vao, color_index, color_size, color_type, normalize_attrib, color_offset);
		
		// This binding index could be switched to quickly change what is drawn
		glVertexArrayAttribBinding(m_vao, position_index, m_attrib_binding_index);
		glVertexArrayAttribBinding(m_vao, color_index, m_attrib_binding_index);
		
		// This could be switched to change which buffer backs the VAO's data
		glVertexArrayVertexBuffer(m_vao, m_attrib_binding_index, m_buffers[0], m_attrib_offset, m_attrib_stride);

		// Other OpenGL settings
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	virtual void render(double current_time)
	{
		// Render code
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		m_shader->use();
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(*vertices));
	};

	const GLuint m_attrib_stride{ sizeof(GLfloat) * 6 };
	const GLuint m_attrib_offset{ 0 };
	const GLuint m_attrib_binding_index{ 0 };
	bool m_buffer_zero{ false };
	GLuint m_vao { 0 };
	GLuint m_buffers[2]{ 0 };
	GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
    std::unique_ptr<GlslProgram>  m_shader;
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app { new MultipleAttributeExample};
	app->run();
}
