// Rendering to a texture using FBOs

#include <memory>
#include <vector>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

// Cube
const GLfloat cube_vertices[]{
	// Positions          // UVs
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

class FboExample : public Application
{
private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_fbo;
	GLuint m_color_buffer_texture;
	GLuint m_depth_buffer_texture;
	const GLuint m_fbo_width_height{ 800 };
	const GLuint m_num_vertices{ 36 };
	const GLfloat m_depth_reset_val{ 1.0f };
	const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
	const std::vector<GLfloat> m_clear_color{ 0.2f, 0.0f, 0.2f, 1.0f };
	Camera m_camera{ glm::vec3{0, 0, 5} };
	std::unique_ptr<GlslProgram> m_shader;
	std::unique_ptr<GlslProgram> m_shader2;

	virtual void setup() override
	{
		// Create shaders
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/cube.vert").fragment("../assets/shaders/cube.frag")});
		m_shader2.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/cube2.vert").fragment("../assets/shaders/cube2.frag")});

		// Cube vertex attribute parameters
		const GLuint elements_per_face{ 5 };

		// Positions
		const GLuint position_index{ 0 };
		const GLuint position_size{ 3 };
		const GLenum position_type{ GL_FLOAT };
		const GLboolean position_normalize{ GL_FALSE };
		const GLuint position_offset_in_buffer{ 0 };

		// UVs
		const GLuint uv_index{ 1 };
		const GLuint uv_size{ 2 };
		const GLenum uv_type{ GL_FLOAT };
		const GLboolean uv_normalize{ GL_FALSE };
		const GLuint uv_offset_in_buffer{ sizeof(GLfloat) * position_size };
		
		// Cube vertex buffer attributes
		const GLuint binding_index{ 0 };
		const GLuint offset{ 0 };
		const GLuint element_stride{ sizeof(GLfloat) * elements_per_face };

		// Setup the cube VBO and its data store
		const GLuint flags{ 0 };
		glCreateBuffers(1, &m_vbo );
		glNamedBufferStorage(m_vbo, sizeof(cube_vertices), cube_vertices, flags);

		// Setup and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Set attributes in the VAO
		glEnableVertexArrayAttrib(m_vao, position_index);
		glEnableVertexArrayAttrib(m_vao, uv_index);
		
		glVertexArrayAttribFormat(m_vao, position_index, position_size, position_type, position_normalize, position_offset_in_buffer);
		glVertexArrayAttribFormat(m_vao, uv_index, uv_size, uv_type, uv_normalize, uv_offset_in_buffer);
		
		glVertexArrayAttribBinding(m_vao, position_index, binding_index);
		glVertexArrayAttribBinding(m_vao, uv_index, binding_index);

		glVertexArrayVertexBuffer(m_vao, binding_index, m_vbo, offset, element_stride);

		// Create an FBO
		glCreateFramebuffers(1, &m_fbo);

		// Create color buffer texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_color_buffer_texture);
		glTextureStorage2D(m_color_buffer_texture, 1, GL_RGB8, m_fbo_width_height, m_fbo_width_height);
		glTextureParameteri(m_color_buffer_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_color_buffer_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Create depth buffer texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_buffer_texture);
		glTextureStorage2D(m_depth_buffer_texture, 1, GL_DEPTH_COMPONENT32F, m_fbo_width_height, m_fbo_width_height);

		// Attach buffers
		glNamedFramebufferTexture(m_fbo, GL_COLOR_ATTACHMENT0, m_color_buffer_texture, 0);
		glNamedFramebufferTexture(m_fbo, GL_DEPTH_ATTACHMENT, m_depth_buffer_texture, 0);

		// This the default (unnecessary here because we only have one output in the FBO frag shader)
		static const GLenum draw_buffers[]{ GL_COLOR_ATTACHMENT0 };
		glNamedFramebufferDrawBuffers(m_fbo, 1, draw_buffers);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
	}

	virtual void render(double current_time) override
	{
		// Bind the member FBO
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glViewport(0, 0, m_fbo_width_height, m_fbo_width_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
		glClearBufferfv(GL_DEPTH, 0, &m_depth_reset_val);

		// Calculate and set cube uniforms
		glm::mat4 model_matrix{ glm::mat4{ 1.0 } };
		model_matrix = glm::rotate(model_matrix, static_cast<GLfloat>(current_time), m_world_up);
		m_shader->use();
		m_shader->uniform("u_model_view_matrix", m_camera.get_view_matrix() * model_matrix);
		m_shader->uniform("u_projection_matrix", m_camera.get_proj_matrix());
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);

		// Return to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTextureUnit(0, m_color_buffer_texture);
        glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
		glClearBufferfv(GL_DEPTH, 0, &m_depth_reset_val);

		// Set uniforms and render textured cube
		m_shader2->use();
		model_matrix = glm::mat4{ 1.0f };
		model_matrix = glm::rotate(model_matrix, static_cast<GLfloat>(current_time / 2.0), m_world_up);
		m_shader2->uniform("u_model_view_matrix", m_camera.get_view_matrix() * model_matrix);
		m_shader2->uniform("u_projection_matrix", m_camera.get_proj_matrix());
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
	};
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new FboExample };
	app->run();
}