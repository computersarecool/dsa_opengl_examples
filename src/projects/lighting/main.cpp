// Standard library
#include <iostream>
#include <string>

// Dependencies
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "shader.h"
#include "camera.h"

static const GLfloat cube_vertices[]{
	// Positions          // Normals           // UVs
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

class LightingExample : public Application
{
private:
	virtual void on_key(int key, int action)
	{
		Application::on_key(key, action);

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
			m_camera.process_keyboard(Camera_Movement::FORWARD, m_delta_time);
		else if (key == GLFW_KEY_S && action == GLFW_PRESS)
			m_camera.process_keyboard(Camera_Movement::BACKWARD, m_delta_time);
		else if (key == GLFW_KEY_A && action == GLFW_PRESS)
			m_camera.process_keyboard(Camera_Movement::LEFT, m_delta_time);
		else if (key == GLFW_KEY_D && action == GLFW_PRESS)
			m_camera.process_keyboard(Camera_Movement::RIGHT, m_delta_time);
	}

	virtual void on_mouse_move(GLdouble x_pos, GLdouble y_pos)
	{
		// Avoid initial "jump" movement
		if (m_first_mouse)
		{
			m_last_x = x_pos;
			m_last_y = y_pos;
			m_first_mouse = GL_FALSE;
		}

		GLdouble x_delta{ x_pos - m_last_x };
		GLdouble y_delta{ y_pos - m_last_y };
		m_camera.process_mouse_movement(x_delta, y_delta);
		m_last_x = x_pos;
		m_last_y = y_pos;
	}

	virtual void on_mouse_wheel(GLdouble x_offset, GLdouble  y_offset)
	{
		m_camera.process_mouse_scroll(y_offset);
	}

	virtual void setup()
	{
		// Create shaders
		std::string this_path = get_parent_directory();
		m_cube_shader = Shader{ (this_path + "/shaders/phong.vert").c_str(), (this_path + "/shaders/phong.frag").c_str() };
		m_lamp_shader = Shader{ (this_path + "/shaders/lamp.vert").c_str(), (this_path + "/shaders/lamp.frag").c_str() };

		// Create the cube shape VBO (this is used for both the cube and lamp)
		const GLuint flags{ 0 };
		glCreateBuffers(1, &m_cube_vbo);
		glNamedBufferStorage(m_cube_vbo, sizeof(cube_vertices), cube_vertices, flags);

		// Attributes for the cube shape
		const GLuint floats_per_face{ 8 };
		const GLuint position_size{ 3 };
		const GLuint normal_size{ 3 };
		const GLuint uv_size{ 2 };
		const GLenum type{ GL_FLOAT };
		const GLboolean normalize{ GL_FALSE };
		const GLuint stride{ sizeof(GLfloat) * floats_per_face };
		const GLuint position_offset{ 0 };
		const GLuint normal_offset{ sizeof(GLfloat) * position_size };
		const GLuint uv_offset{ sizeof(GLfloat) * position_size + sizeof(GLfloat) * normal_size };
		const GLuint binding_index{ 0 };
		const GLuint position_index{ 0 };
		const GLuint normal_index{ 1 };
		const GLuint uv_index{ 2 };

		// Set cube attributes
		glCreateVertexArrays(1, &m_cube_vao);

		glEnableVertexArrayAttrib(m_cube_vao, position_index);
		glEnableVertexArrayAttrib(m_cube_vao, normal_index);
		glEnableVertexArrayAttrib(m_cube_vao, uv_index);

		glVertexArrayAttribFormat(m_cube_vao, position_index, position_size, type, normalize, position_offset);
		glVertexArrayAttribFormat(m_cube_vao, normal_index, normal_size, type, normalize, normal_offset);
		glVertexArrayAttribFormat(m_cube_vao, uv_index, uv_size, type, normalize, uv_offset);

		glVertexArrayAttribBinding(m_cube_vao, position_index, binding_index);
		glVertexArrayAttribBinding(m_cube_vao, normal_index, binding_index);
		glVertexArrayAttribBinding(m_cube_vao, uv_index, binding_index);

		glVertexArrayVertexBuffer(m_cube_vao, binding_index, m_cube_vbo, position_offset, stride);

		// Set light attributes
		glCreateVertexArrays(1, &m_lamp_vao);
		glEnableVertexArrayAttrib(m_lamp_vao, position_index);
		glVertexArrayAttribFormat(m_lamp_vao, position_index, position_size, type, normalize, position_offset);
		glVertexArrayAttribBinding(m_lamp_vao, position_index, binding_index);
		glVertexArrayVertexBuffer(m_lamp_vao, binding_index, m_cube_vbo, position_offset, stride);

		// Create a sampler
		GLuint sampler_name;
		GLuint starting_texture_unit{ 0 };
		glCreateSamplers(1, &sampler_name);
		glSamplerParameteri(sampler_name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(sampler_name, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameteri(sampler_name, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(sampler_name, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindSampler(starting_texture_unit, sampler_name);

		// Load diffuse and specular maps
		std::string diffuse_map_path{ (this_path + "/assets/container2.jpg") };
		std::string specular_map_path{ (this_path + "/assets/container2_specular.jpg")};
		GLint diffuse_width;
		GLint specular_width;
		GLint diffuse_height;
		GLint specular_height;
		GLint diffuse_nr_channels;
		GLint specular_nr_channels;
		GLubyte* diffuse_data = stbi_load(diffuse_map_path.c_str(), &diffuse_width, &diffuse_height, &diffuse_nr_channels, 0);
		if (!diffuse_data)
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		GLubyte* specular_data = stbi_load(specular_map_path.c_str(), &specular_width, &specular_height, &specular_nr_channels, 0);
		if (!specular_data)
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		// Generate OpenGL textures
		GLuint box_textures[2];
		const GLuint diffuse_map_index{ 0 };
		const GLuint specular_map_index{ 1 };
		GLint diffuse_map_size;
		GLint specular_map_size;
		GLsizei levels = 1;
		GLint level_to_load = 0;
		GLint y_offset = 0;
		GLint x_offset = 0;
		glCreateTextures(GL_TEXTURE_2D, 2, box_textures);
		glTextureStorage2D(box_textures[diffuse_map_index], levels, GL_RGB8, diffuse_width, diffuse_height);
		glTextureStorage2D(box_textures[specular_map_index], levels, GL_RGB8, diffuse_width, diffuse_height);
		glTextureSubImage2D(box_textures[diffuse_map_index], level_to_load, x_offset, y_offset, diffuse_width, diffuse_height, GL_RGB, GL_UNSIGNED_BYTE, diffuse_data);
		glTextureSubImage2D(box_textures[specular_map_index], level_to_load, x_offset, y_offset, specular_width, specular_height, GL_RGB, GL_UNSIGNED_BYTE, specular_data);
		glBindTextureUnit(starting_texture_unit, box_textures[diffuse_map_index]);
		glBindTextureUnit(starting_texture_unit + 1, box_textures[specular_map_index]);
		stbi_image_free(diffuse_data);
		stbi_image_free(specular_data);

		// Global OpenGL state
		glEnable(GL_DEPTH_TEST);
	}

	virtual void render(double current_time)
	{
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

		// Objects in scene
		m_cube_shader.use();

		// Update and set uniforms
		m_model_matrix = glm::mat4{ 1.0f };
		m_view_matrix = m_camera.get_view_matrix();
		m_projection_matrix = m_camera.get_proj_matrix();
		m_normal_matrix = glm::inverseTranspose(glm::mat3(m_model_matrix));

		// Light uniforms
		m_cube_shader.set_vec3("uLightColor", m_light_color);
		m_cube_shader.set_vec3("uLightPos", m_camera.get_pos());
		m_cube_shader.set_vec3("uLightDirection", m_camera.get_front());
		m_cube_shader.set_float("uLightCutoff", m_light_cutoff);
		m_cube_shader.set_float("uLightOuterCutoff", m_light_outer_cutoff);

		// Material uniforms
		m_cube_shader.set_vec3("uSpecularColor", m_specular_color);
		m_cube_shader.set_vec4("uDiffuseColor", m_diffuse_color);
		m_cube_shader.set_vec4("uAmbientColor", m_ambient_color);
		m_cube_shader.set_vec3("uShadowColor", m_shadow_color);
		m_cube_shader.set_float("uShadowStrength", m_shadow_strength);
		m_cube_shader.set_float("uShininess", m_shininess);

		// Other uniforms
		m_cube_shader.set_vec3("uCameraPos", m_camera.get_pos());
		m_cube_shader.set_mat4("uModelMatrix", m_model_matrix);
		m_cube_shader.set_mat4("uViewMatrix", m_view_matrix);
		m_cube_shader.set_mat4("uProjectionMatrix", m_projection_matrix);
		m_cube_shader.set_mat3("uNormalMatrix", m_normal_matrix);
		m_cube_shader.set_vec4("uVertexColor", m_vertex_color);

		glBindVertexArray(m_cube_vao);
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);

		// Lamps in scene
		m_lamp_shader.use();
		m_model_matrix = glm::mat4{ 1.0f };
		m_model_matrix = glm::translate(m_model_matrix, m_light_pos);
		m_model_matrix = glm::scale(m_model_matrix, glm::vec3{ 0.2f });

		m_lamp_shader.set_mat4("uModelMatrix", m_model_matrix);
		m_lamp_shader.set_mat4("uViewMatrix", m_view_matrix);
		m_lamp_shader.set_mat4("uProjectionMatrix", m_projection_matrix);

		glBindVertexArray(m_lamp_vao);
		glDrawArrays(GL_TRIANGLES, 0, m_num_vertices);
	};

	// Application state	
	GLfloat m_delta_time{ 0.0f };
	GLfloat m_last_frame{ 0.0f };
	GLboolean m_first_mouse{ GL_TRUE };
	GLfloat m_last_x{ m_info.window_width / 2.0f };
	GLfloat m_last_y{ m_info.window_height / 2.0f };
	const GLuint m_num_vertices{ 36 };
	Camera m_camera{ glm::vec3{ 0.0f, 0.0f, 3.0f } };

	// OpenGL handles
	GLuint m_lamp_vao;
	Shader m_lamp_shader;
	GLuint m_cube_vao;
	GLuint m_cube_vbo;
	Shader m_cube_shader;
	GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };

	// Uniforms
	glm::mat4 m_model_matrix;
	glm::mat4 m_view_matrix;
	glm::mat4 m_projection_matrix;
	glm::mat3 m_normal_matrix;
	glm::vec4 m_vertex_color{1.0f, 0.5f, 0.31f, 1.0f };
	glm::vec3 m_light_color{ 0.5f };
	glm::vec3 m_light_pos{0.0f, 0.25f, 3.0f };
	GLfloat m_light_cutoff{ glm::cos(glm::radians(12.5f)) };
	GLfloat m_light_outer_cutoff{ glm::cos(glm::radians(17.5f)) };
	glm::vec3 m_specular_color{ 1.0f };
	glm::vec4 m_diffuse_color{ 0.7, 0.7, 0.7, 1.0 };
	glm::vec4 m_ambient_color{ 0.0, 0.0, 0.0, 1.0 };
	glm::vec3 m_shadow_color{ 0.0f };
	GLfloat m_shadow_strength{ 0.0f };
	GLfloat m_shininess{ 100.0 };
};

int main(int argc, char* argv[])
{
	Application* my_app = new LightingExample;
	my_app->run();
	delete my_app;
	return 0;
}
