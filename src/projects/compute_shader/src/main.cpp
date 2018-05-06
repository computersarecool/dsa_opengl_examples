// This renders a cube to an FBO then uses a compute shader to invert that image

#include <memory>
#include <vector>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

// Cube vertices
static const GLfloat cube_vertices[]{
       // Positions       // Normals
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

class ComputeShaderExample : public Application
{
private:
	GLuint m_cube_vao;
	GLuint m_full_screen_quad_vao;
	GLuint m_cube_vbo;
	GLuint m_src_fbo;
	GLuint m_color_texture;
	GLuint m_depth_texture;
	GLuint m_second_color_texture;
	Camera m_camera{ glm::vec3{ 0, 0, 5 } };
	const GLuint m_vertices_per_cube{ sizeof(cube_vertices) / sizeof(*cube_vertices) };
	const int m_number_cubes{ 9 };
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const std::vector<GLfloat> m_clear_color{ 0.2f, 0.0f, 0.2f, 1.0f };
	const GLfloat m_depth_reset_val{ 1.0f };
	std::unique_ptr<GlslProgram> m_cube_shader;
	std::unique_ptr<GlslProgram> m_full_screen_quad_shader;
	std::unique_ptr<GlslProgram> m_compute_shader;

	virtual void set_info() override
	{
		Application::set_info();
		m_info.title = "Compute shader example";
	}

	void load_shaders()
	{
		m_cube_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/cube.vert").fragment("../assets/shaders/cube.frag") });
		m_full_screen_quad_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/full_screen_quad.vert").fragment("../assets/shaders/full_screen_quad.frag") });
		m_compute_shader.reset(new GlslProgram{ GlslProgram::Format().compute("../assets/shaders/shader.comp") });
	}

	void setup_cube()
	{
		// Vertex attribute parameters
		const GLuint elements_per_face{ 6 };

		// Positions
        const GLuint position_index{ 0 };
		const GLenum position_type{ GL_FLOAT };
        const GLuint position_size{ 3 };
        const GLboolean position_normalize{ GL_FALSE };
        const GLuint position_offset_in_buffer{ 0 };

        // Normals
        const GLuint normal_index{ 1 };
        const GLuint normal_size{ 3 };
        const GLenum normal_type{ GL_FLOAT };
        const GLboolean normal_normalize{ GL_FALSE };
		const GLuint normal_offset_in_buffer{ sizeof(GLfloat) * position_size };

		// Vertex buffer attributes
		const GLuint binding_index{ 0 };
		const GLuint offset{ 0 };
		const GLuint element_stride{ sizeof(GLfloat) * elements_per_face };

		// Set up VBO and its data store
		const GLuint flags{ 0 };
		glCreateBuffers(1, &m_cube_vbo);
		glNamedBufferStorage(m_cube_vbo, sizeof(cube_vertices), cube_vertices, flags);

		// Set up cube VAO
		glCreateVertexArrays(1, &m_cube_vao);
		
		glEnableVertexArrayAttrib(m_cube_vao, position_index);
        glVertexArrayAttribFormat(m_cube_vao, position_index, position_size, position_type, position_normalize, position_offset_in_buffer);
        glVertexArrayAttribBinding(m_cube_vao, position_index, binding_index);

        glEnableVertexArrayAttrib(m_cube_vao, normal_index);
        glVertexArrayAttribFormat(m_cube_vao, normal_index, normal_size, normal_type, normal_normalize, normal_offset_in_buffer);
        glVertexArrayAttribBinding(m_cube_vao, normal_index, binding_index);

		glVertexArrayVertexBuffer(m_cube_vao, binding_index, m_cube_vbo, offset, element_stride);
	}

	void setup_textures_and_buffers()
	{
		// Create a framebuffer
		const std::vector<GLenum> draw_buffers{ GL_COLOR_ATTACHMENT0 };
		glCreateFramebuffers(1, &m_src_fbo);
		glNamedFramebufferDrawBuffers(m_src_fbo, 1, draw_buffers.data());

		// Create depth texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_texture);
		glTextureStorage2D(m_depth_texture, 10, GL_DEPTH_COMPONENT32F, m_info.window_width, m_info.window_height);
		glTextureParameteri(m_depth_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_depth_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Create the color texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_color_texture);
		glTextureStorage2D(m_color_texture, 1, GL_RGBA32F, m_info.window_width, m_info.window_height);
		glTextureParameteri(m_color_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_color_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Create the second color texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_second_color_texture);
		glTextureStorage2D(m_second_color_texture, 1, GL_RGBA32F, m_info.window_width, m_info.window_height);
		glTextureParameteri(m_second_color_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_second_color_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Attach textures
		glNamedFramebufferTexture(m_src_fbo, GL_DEPTH_ATTACHMENT, m_depth_texture, 0);
		glNamedFramebufferTexture(m_src_fbo, GL_COLOR_ATTACHMENT0, m_color_texture, 0);

		// Set up full screen quad VAO
		glCreateVertexArrays(1, &m_full_screen_quad_vao);
	}

	virtual void setup() override
	{
		load_shaders();
		setup_cube();
		setup_textures_and_buffers();
	}

	virtual void render(double current_time) override
	{
		// Draw scene into the src FBO
		m_cube_shader->use();
		glBindVertexArray(m_cube_vao);
		glBindFramebuffer(GL_FRAMEBUFFER, m_src_fbo);
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
		glClearBufferfv(GL_DEPTH, 0, &m_depth_reset_val);
		glEnable(GL_DEPTH_TEST);
		
		for (int index{ 0 }; index != m_number_cubes; ++index)
		{
			glm::mat4 model_matrix{ glm::mat4{ 1.0 } };
			model_matrix = glm::translate(model_matrix, glm::vec3{ -1.5, 0, 0 });
			model_matrix = glm::translate(model_matrix, glm::vec3{index, static_cast<float>(index) / 5, index * -2 });
			model_matrix = glm::rotate(model_matrix, static_cast<GLfloat>(current_time), m_world_up);
			m_cube_shader->uniform("u_model_view_matrix", m_camera.get_view_matrix() * model_matrix);
			m_cube_shader->uniform("u_projection_matrix", m_camera.get_proj_matrix());
			glDrawArrays(GL_TRIANGLES, 0, m_vertices_per_cube);
		}

		// Compute shader
		m_compute_shader->use();
		glBindImageTexture(0, m_color_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, m_second_color_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(static_cast<GLuint>(m_info.window_width / 32), static_cast<GLuint>(m_info.window_height / 32), 1);

		// Draw full screen quad
		m_full_screen_quad_shader->use();
		glBindVertexArray(m_full_screen_quad_vao);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glBindTextureUnit(0, m_second_color_texture);
		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	};
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new ComputeShaderExample };
	app->run();
}
