// A basic cube example
#include "glm/glm/gtc/matrix_transform.hpp"

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
		// Setup what is neccesary to draw the cubes
		m_cube_shader = Shader{ "../assets/shaders/cube.vert", "../assets/shaders/cube.frag" };

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
		// NOTE: We are leaving the VAO bound

		// Setup what is neccesary for the textures
		glCreateFramebuffers(1, &m_fbo);


		glCreateTextures(GL_TEXTURE_2D, 1, &m_color_texture);
		glTextureStorage2D(m_color_texture, 1, GL_RGBA32F, m_fbo_size, m_fbo_size);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_temp_texture);
		glTextureStorage2D(m_temp_texture, 1, GL_RGBA32F, m_fbo_size, m_fbo_size);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_color_texture, 0);
	}

	virtual void render(double current_time)
	{
		// Set OpenGL state
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Draw cubes
		m_cube_shader.use();
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
	};

	// Member variables
	Shader m_cube_shader;
	GLuint m_cube_vao;
	GLuint m_cube_vbo;
	GLuint m_fbo;
	GLuint m_color_texture;
	GLuint m_temp_texture;
	Camera m_camera{ glm::vec3{ 0, 0, 5 } };
	const GLuint m_vertices_per_cube{ 36 };
	const int m_number_cubes{ 9 };
	const int m_fbo_size{ 2048 };
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
	Application* my_app = new BasicCubeExample;
	my_app->run();
	delete my_app;
}
