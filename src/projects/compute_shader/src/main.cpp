// Compute shader example
#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "shader.h"
#include "compute_shader.h"
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

class ComputeShaderExample : public Application
{
private:
	virtual void set_info()
	{
		Application::set_info();
		m_info.title = "Compute shader example";
	}

	void load_shaders()
	{
		m_cube_shader = Shader{ "../assets/shaders/cube.vert", "../assets/shaders/cube.frag" };
		m_full_screen_quad_shader = Shader{ "../assets/shaders/full_screen_quad.vert", "../assets/shaders/full_screen_quad.frag" };
		m_compute_shader = ComputeShader{ "../assets/shaders/shader.comp" };
	}

	void setup_cube()
	{
		// Vertex attribute parameters
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

		// Vertex buffer attributes
		const GLuint binding_index{ 0 };
		const GLuint offset{ 0 };
		const GLuint element_stride{ sizeof(GLfloat) * elements_per_face };

		// Setup VBO and its data store
		const GLuint flags{ 0 };
		glCreateBuffers(1, &m_cube_vbo);
		glNamedBufferStorage(m_cube_vbo, sizeof(vertices), vertices, flags);

		// Setup and bind a VAO
		glCreateVertexArrays(1, &m_cube_vao);
		glBindVertexArray(m_cube_vao);

		// Set attributes in the VAO
		glEnableVertexArrayAttrib(m_cube_vao, position_index);
		glEnableVertexArrayAttrib(m_cube_vao, normal_index);

		glVertexArrayAttribFormat(m_cube_vao, position_index, position_size, position_type, position_normalize, position_offset_in_buffer);
		glVertexArrayAttribFormat(m_cube_vao, normal_index, normal_size, normal_type, normal_normalize, normal_offset_in_buffer);

		glVertexArrayAttribBinding(m_cube_vao, position_index, binding_index);
		glVertexArrayAttribBinding(m_cube_vao, normal_index, binding_index);

		glVertexArrayVertexBuffer(m_cube_vao, binding_index, m_cube_vbo, offset, element_stride);
	}

	virtual void setup()
	{
		load_shaders();
		setup_cube();

		// Create framebuffer
		glCreateFramebuffers(1, &m_depth_fbo);

		// Create depth buffer texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_texture);
		glTextureStorage2D(m_depth_texture, 10, GL_DEPTH_COMPONENT32F, m_depth_fbo_size, m_depth_fbo_size);

		glTextureParameteri(m_depth_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_depth_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		// Create the color texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_color_texture);
		glTextureStorage2D(m_color_texture, 1, GL_RGB32F, m_depth_fbo_size, m_depth_fbo_size);
		glTextureParameteri(m_color_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_color_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Create temp texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_temp_texture);
		glTextureStorage2D(m_temp_texture, 1, GL_RGB32F, m_depth_fbo_size, m_depth_fbo_size);
		glTextureParameteri(m_temp_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_temp_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Attach buffer textures
		glNamedFramebufferTexture(m_depth_fbo, GL_DEPTH_ATTACHMENT, m_depth_texture, 0);
		glNamedFramebufferTexture(m_depth_fbo, GL_COLOR_ATTACHMENT0, m_color_texture, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glEnable(GL_DEPTH_TEST);

		// Setup and bind full screen quad VAO
		glCreateVertexArrays(1, &m_full_screen_quad_vao);
		glBindVertexArray(m_full_screen_quad_vao);
	}

	virtual void render(double current_time)
	{
		static const GLenum draw_buffers[]{ GL_COLOR_ATTACHMENT0 };

		// Render scene
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, m_depth_fbo);
		glNamedFramebufferDrawBuffers(m_depth_fbo, 1, draw_buffers);
		glViewport(0, 0, m_depth_fbo_size, m_depth_fbo_size);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfv(GL_DEPTH, 0, &m_depth_reset_val);

		m_cube_shader.use();
		glBindVertexArray(m_cube_vao);
		
		for (int i = 0; i < m_number_cubes; i++)
		{
			glm::mat4 model_matrix{ glm::mat4{ 1.0 } };
			model_matrix = glm::translate(model_matrix, glm::vec3{ -1.5, 0, 0 });
			model_matrix = glm::translate(model_matrix, glm::vec3{i, static_cast<float>(i) / 5, i * -2 });
			model_matrix = glm::rotate(model_matrix, static_cast<GLfloat>(current_time), m_world_up);
			m_cube_shader.set_mat4("uModelViewMatrix", m_camera.get_view_matrix() * model_matrix);
			m_cube_shader.set_mat4("uProjectionMatrix", m_camera.get_proj_matrix());
			glDrawArrays(GL_TRIANGLES, 0, m_vertices_per_cube);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Compute shader
		m_compute_shader.use();
		glBindImageTexture(0, m_color_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, m_temp_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glDispatchCompute(m_info.window_width, 1, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glBindImageTexture(0, m_temp_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, m_color_texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glDispatchCompute(m_info.window_width, 1, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Display
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_color_texture);
		glDisable(GL_DEPTH_TEST);
		m_full_screen_quad_shader.use();
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glBindVertexArray(m_full_screen_quad_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		check_gl_error();
	};

	// Member variables
	Shader m_cube_shader;
	Shader m_full_screen_quad_shader;
	ComputeShader m_compute_shader;
	GLuint m_cube_vao;
	GLuint m_full_screen_quad_vao;
	GLuint m_cube_vbo;
	GLuint m_depth_fbo;
	GLuint m_color_texture;
	GLuint m_depth_texture;
	GLuint m_temp_texture;
	Camera m_camera{ glm::vec3{ 0, 0, 5 } };
	const GLuint m_vertices_per_cube{ 36 };
	const int m_number_cubes{ 9 };
	const int m_depth_fbo_size{ 800 };
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
	const GLfloat m_depth_reset_val{ 1.0f };
};

int main(int argc, char* argv[])
{
	Application* my_app = new ComputeShaderExample;
	my_app->run();
	delete my_app;
}
