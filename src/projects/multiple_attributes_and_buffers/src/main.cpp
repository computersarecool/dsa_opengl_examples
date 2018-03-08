// This uses mapped buffers to upload data
// The buffer backing the VAO is switched to change vertex data quickly
// The attribute binding could also be switched to change the vertex data quickly
// https://stackoverflow.com/a/21652955/3291506 explains buffer binding indices nicely
// Interactivity: Spacebar toggles which buffer back the active VAO

#include <memory>
#include <vector>

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
	// Positions          // Colors
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
	virtual void on_key(int key, int action) override
	{
		Application::on_key(key, action);

        // Toggle which buffer backs the VAO
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			if (m_buffer_zero)
				glVertexArrayVertexBuffer(m_vao, m_attrib_binding_index, m_buffers[0], m_attrib_offset, m_attrib_stride);

			else
				glVertexArrayVertexBuffer(m_vao, m_attrib_binding_index, m_buffers[1], m_attrib_offset, m_attrib_stride);

            m_buffer_zero = !m_buffer_zero;
        }
	}

	virtual void setup() override
	{
		// Create shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/shader.vert").fragment("../assets/shaders/shader.frag")});

		// Create buffers, map them, upload data, unmap them
		const GLuint flags{ 0 };
		const GLuint buffer_offset{ 0 };

		glCreateBuffers(static_cast<GLsizei>(m_buffers.size()), m_buffers.data());

        // Map buffers and upload data
        const GLsizeiptr buffer_size = sizeof(vertices);
        for (int i { 0 }; i < m_buffers.size(); ++i)
        {
            glNamedBufferStorage(m_buffers[i], buffer_size, nullptr, GL_MAP_WRITE_BIT);
            void* ptr = glMapNamedBufferRange(m_buffers[i], buffer_offset, buffer_size, GL_MAP_WRITE_BIT);

            if (!i)
            {
                memcpy(ptr, vertices, buffer_size);
            }
            else
            {
                memcpy(ptr, vertices2, buffer_size);
            }
            glUnmapNamedBuffer(m_buffers[i]);
        }

		//  Vertex attribute parameters
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

        // Setup the VAO
		glCreateVertexArrays(1, &m_vao);
		glEnableVertexArrayAttrib(m_vao, position_index);
		glEnableVertexArrayAttrib(m_vao, color_index);
		glVertexArrayAttribFormat(m_vao, position_index, position_size, position_type, normalize_attrib, position_offset);
		glVertexArrayAttribFormat(m_vao, color_index, color_size, color_type, normalize_attrib, color_offset);
		
		// This binding index could also be switched for quick changing of data
		glVertexArrayAttribBinding(m_vao, position_index, m_attrib_binding_index);
		glVertexArrayAttribBinding(m_vao, color_index, m_attrib_binding_index);
		
		// Start with the first VBO backing the VAO.
        // This is switched when spacebar is pressed
		glVertexArrayVertexBuffer(m_vao, m_attrib_binding_index, m_buffers[0], m_attrib_offset, m_attrib_stride);
	}

	virtual void render(double current_time) override
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
    const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
    std::vector<GLuint> m_buffers{ 0, 0 };
    std::unique_ptr<GlslProgram> m_shader;
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app { new MultipleAttributeExample};
	app->run();
}
