// An example that uses a displacment map to offset vertices and tesselation
// This uses an instanced quad with vertices embedded in the shader (there are no vertex attributes)

#include <iostream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

class DisplacmentMapTesselationExample : public Application
{
private:
	virtual void set_info() override
	{
		Application::set_info();	
		m_info.title = "Tesselation Displacement Map";
	}

	virtual void on_key(int key, int action) override
	{
		Application::on_key(key, action);
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			m_show_wireframe = !m_show_wireframe;
		}
	}

	virtual void setup() override
	{
		// Create shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/terrain_disp.vert").fragment("../assets/shaders/terrain_disp.frag").tess_control("../assets/shaders/terrain_disp.tesc").tess_eval("../assets/shaders/terrain_disp.tese")});

		// Create and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Load images
		// TODO: Make this a function
		GLint displacement_map_width;
		GLint displacement_map_height;
		GLint displacement_map_channels;
		GLubyte* displacement_map_data = stbi_load(m_displacement_map_path.c_str(), &displacement_map_width, &displacement_map_height, &displacement_map_channels, 0);
		if (!displacement_map_data)
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		GLint color_map_width;
		GLint color_map_height;
		GLint color_map_channels;
		GLubyte* color_map_data = stbi_load(m_color_map_path.c_str(), &color_map_width, &color_map_height, &color_map_channels, 0);
		if (!color_map_data)
		{
			std::cout << "Failed to load texture" << std::endl;
		}

		// Set up sampler
		GLuint repeat_linear_sampler;
		glCreateSamplers(1, &repeat_linear_sampler);
		glSamplerParameteri(repeat_linear_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(repeat_linear_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameteri(repeat_linear_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(repeat_linear_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindSampler(0, repeat_linear_sampler);
		glBindSampler(1, repeat_linear_sampler);

		// Set up textures
		// TODO: Make this a function
		glCreateTextures(GL_TEXTURE_2D, 1, &m_displacement_texture);
		glTextureStorage2D(m_displacement_texture, 1, GL_RGB8, displacement_map_width, displacement_map_height);
		glTextureSubImage2D(m_displacement_texture, 0, 0, 0, displacement_map_width, displacement_map_height, GL_RGB, GL_UNSIGNED_BYTE, displacement_map_data);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_color_texture);
		glTextureStorage2D(m_color_texture, 1, GL_RGB8, color_map_width, color_map_height);
		glTextureSubImage2D(m_color_texture, 0, 0, 0, color_map_width, color_map_height, GL_RGB, GL_UNSIGNED_BYTE, color_map_data);

		// Bind textures
		glBindTextureUnit(0, m_displacement_texture);
		glBindTextureUnit(1, m_color_texture);
		
		// Free image data
		stbi_image_free(displacement_map_data);
		stbi_image_free(color_map_data);		
	}

	virtual void render(double current_time) override
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

		// Move camera position over time
		// The magic numbers here just offset it arbitrarily
		m_slow_time = static_cast<GLfloat>(current_time) * m_time_divisor;
		m_camera_rotation_value = sinf(m_slow_time * 5.0f) * 15.0f;
		m_camera_y_value = cosf(m_slow_time * 5.0f) + 5.0f;
		m_camera.set_position(sinf(m_slow_time) * m_camera_rotation_value, m_camera_y_value, cosf(m_slow_time) * m_camera_rotation_value);

		// Set uniforms in shader
		m_shader->use();
		view_matrix = m_camera.get_view_matrix();
		proj_matrix = m_camera.get_proj_matrix();
		m_shader->uniform("uModelViewProjectionMatrix", proj_matrix * view_matrix);
		m_shader->uniform("uDisplacementWeight", displace_weight);

		glDrawArraysInstanced(GL_PATCHES, 0, m_vertices_per_patch, m_num_instances);
	};

	// Member variables
	const GLfloat m_time_divisor{ 0.03f };
	GLfloat m_slow_time { 0 };
	GLfloat m_camera_rotation_value { 0 };
	GLfloat m_camera_y_value { 0 };
	GLuint m_vao { 0 };
	GLuint m_displacement_texture { 0 };
	GLuint m_color_texture { 0 };
	const std::string m_displacement_map_path{ "../assets/images/noise.jpg"};
	const std::string m_color_map_path{ "../assets/images/noise_color.jpg" };
	bool m_show_wireframe{ false };
	const GLuint m_vertices_per_patch{ 4 };
	const GLuint m_num_instances{ 64 * 64 };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
	const GLfloat displace_weight{ 6.0 };
	Camera m_camera;
	glm::mat4 view_matrix;
	glm::mat4 proj_matrix;
	std::unique_ptr<GlslProgram> m_shader;
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new DisplacmentMapTesselationExample };
	app->run();
}