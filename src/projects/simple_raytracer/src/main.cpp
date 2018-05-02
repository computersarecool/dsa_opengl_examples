// A raytracer
// TODO: Make the fullscreen quad a separable program

#include <memory>
#include <vector>
#include <random>
#include <ctime>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

class RayTracer : public Application
{
private:
	// UBO structs
	// These use alignas to avoid the need for padding
	struct alignas(16) sphere
	{
		glm::vec4 center{ 0 };
		glm::vec4 color{ 0 };
		float radius{ 1.0 };
	};

	struct light
	{
		glm::vec4 center{ 0 };
	};

	struct alignas(16) plane
	{
		glm::vec4 normal;
		float d;
	};

	struct uniforms_block
	{
		glm::mat4 model_view_matrix;
		glm::mat4 view_matrix;
		glm::mat4 projection_matrix;
	};

	// Scene description
	int m_max_depth{ 2 };
	static const int m_max_recursion_depth{ 5 };
	const int m_max_framebuffer_width{ 2048 };
	const int m_max_framebuffer_height{ 1024 };
	const int m_num_spheres{ 4 };
	const int m_num_lights{ 2 };
	const int m_num_planes{ 6 };

	// UBO binding indices
	const GLuint m_sphere_ubo_binding_index{ 1 };
	const GLuint m_light_ubo_binding_index{ 2 };
	const GLuint m_plane_ubo_binding_index{ 2 };

	// UBO handles
	GLuint m_light_buffer;
	GLuint m_sphere_buffer;
	GLuint m_plane_buffer;
	GLuint m_uniforms_buffer;

	// FBO texture handles
	GLuint m_tex_composite;
	GLuint m_tex_position[m_max_recursion_depth];
	GLuint m_tex_reflected[m_max_recursion_depth];
	GLuint m_tex_reflection_intensity[m_max_recursion_depth];
	GLuint m_tex_refracted[m_max_recursion_depth];
	GLuint m_tex_refraction_intensity[m_max_recursion_depth];

	// GLSL Programs
	std::unique_ptr<GlslProgram> m_prepare_program;
	std::unique_ptr<GlslProgram> m_trace_program;
	std::unique_ptr<GlslProgram> m_blit_program;

	// Other OpenGL handles
	GLuint m_vao{ 0 };
	std::vector<GLuint> m_ray_fbos{ 0, 0, 0, 0, 0 };
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
	glm::vec3 m_view_position{ glm::vec3{ 0, 0, 5 } };
	Camera m_camera{ m_view_position };
	std::vector<glm::vec4> m_sphere_colors;

	virtual void set_info() override
	{
		Application::set_info();
		m_info.title = "Raytracer example";
	}

	virtual void setup() override
	{
		// Create a random number generator
		std::default_random_engine random_engine(time(0));
		std::uniform_real_distribution<float> uniform_dist(0, 1);

		// Randomly color the spheres (ignore alpha component)
		for (int i{ 0 }; i < m_num_spheres; ++i)
		{
			m_sphere_colors.push_back(glm::vec4{ uniform_dist(random_engine), uniform_dist(random_engine), uniform_dist(random_engine), 0 });
		}

		// Load shaders
		m_prepare_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/trace_prepare.vert").fragment("../assets/shaders/trace_prepare.frag") });
		m_trace_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/raytracer.vert").fragment("../assets/shaders/raytracer.frag") });
		m_blit_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/blit.vert").fragment("../assets/shaders/blit.frag") });

		// Create buffers and bind UBOs
		glCreateBuffers(1, &m_uniforms_buffer);
		glNamedBufferStorage(m_uniforms_buffer, sizeof(uniforms_block), nullptr, GL_MAP_WRITE_BIT);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_sphere_ubo_binding_index, m_sphere_buffer, 0, m_num_spheres * sizeof(sphere));

		glCreateBuffers(1, &m_sphere_buffer);
		glNamedBufferStorage(m_sphere_buffer, m_num_spheres * sizeof(sphere), nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_sphere_ubo_binding_index, m_sphere_buffer, 0, m_num_spheres * sizeof(sphere));

		glCreateBuffers(1, &m_light_buffer);
		glNamedBufferStorage(m_light_buffer, m_num_lights * sizeof(light), nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_light_ubo_binding_index, m_light_buffer, 0, m_num_lights * sizeof(light));

		glCreateBuffers(1, &m_plane_buffer);
		glNamedBufferStorage(m_plane_buffer, m_num_planes * sizeof(plane), nullptr, GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_plane_ubo_binding_index, m_plane_buffer, 0, m_num_planes * sizeof(plane));

		// Create and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Create FBOs
		glCreateFramebuffers(static_cast<GLsizei>(m_ray_fbos.size()), m_ray_fbos.data());

		// Create FBO textures
		glCreateTextures(GL_TEXTURE_2D, 1, &m_tex_composite);
		glTextureStorage2D(m_tex_composite, 1, GL_RGB16F, m_max_framebuffer_width, m_max_framebuffer_height);

		glCreateTextures(GL_TEXTURE_2D, m_max_recursion_depth, m_tex_position);
		glCreateTextures(GL_TEXTURE_2D, m_max_recursion_depth, m_tex_reflected);
		glCreateTextures(GL_TEXTURE_2D, m_max_recursion_depth, m_tex_refracted);
		glCreateTextures(GL_TEXTURE_2D, m_max_recursion_depth, m_tex_reflection_intensity);
		glCreateTextures(GL_TEXTURE_2D, m_max_recursion_depth, m_tex_refraction_intensity);

		// TODO: Use a sampler
		// Set FBO textures and texture parameters
		for (int i{ 0 }; i < m_max_recursion_depth; ++i)
		{
			glNamedFramebufferTexture(m_ray_fbos[i], GL_COLOR_ATTACHMENT0, m_tex_composite, 0);

			glTextureStorage2D(m_tex_position[i], 1, GL_RGB32F, m_max_framebuffer_width, m_max_framebuffer_height);
			glTextureParameteri(m_tex_position[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_tex_position[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glNamedFramebufferTexture(m_ray_fbos[i], GL_COLOR_ATTACHMENT1, m_tex_position[i], 0);

			glTextureStorage2D(m_tex_reflected[i], 1, GL_RGB16F, m_max_framebuffer_width, m_max_framebuffer_height);
			glTextureParameteri(m_tex_reflected[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_tex_reflected[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glNamedFramebufferTexture(m_ray_fbos[i], GL_COLOR_ATTACHMENT2, m_tex_reflected[i], 0);

			glTextureStorage2D(m_tex_refracted[i], 1, GL_RGB16F, m_max_framebuffer_width, m_max_framebuffer_height);
			glTextureParameteri(m_tex_refracted[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_tex_refracted[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glNamedFramebufferTexture(m_ray_fbos[i], GL_COLOR_ATTACHMENT3, m_tex_refracted[i], 0);

			glTextureStorage2D(m_tex_reflection_intensity[i], 1, GL_RGB16F, m_max_framebuffer_width, m_max_framebuffer_height);
			glTextureParameteri(m_tex_reflection_intensity[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_tex_reflection_intensity[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glNamedFramebufferTexture(m_ray_fbos[i], GL_COLOR_ATTACHMENT4, m_tex_reflection_intensity[i], 0);

			glTextureStorage2D(m_tex_refraction_intensity[i], 1, GL_RGB16F, m_max_framebuffer_width, m_max_framebuffer_height);
			glTextureParameteri(m_tex_refraction_intensity[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_tex_refraction_intensity[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glNamedFramebufferTexture(m_ray_fbos[i], GL_COLOR_ATTACHMENT5, m_tex_refraction_intensity[i], 0);
		}
	}

	virtual void render(double current_time) override
	{
		// Set uniform data
		glm::mat4 model_matrix = glm::mat4{ 1.0 };
		glm::mat4 view_matrix = m_camera.get_view_matrix();

		// TODO: This is not mapped correctly
		auto uniforms_ptr = static_cast<uniforms_block*>(glMapNamedBufferRange(m_uniforms_buffer, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT));
		uniforms_ptr->model_view_matrix = view_matrix * model_matrix;
		uniforms_ptr->view_matrix = view_matrix;
		uniforms_ptr->projection_matrix = m_camera.get_proj_matrix();
		glUnmapNamedBuffer(m_uniforms_buffer);

		// The alternative to mapping a buffer could be using glNamedBufferSubData
		// i.e. glNamedBufferSubData(m_sphere_buffer, 0, sizeof(glm::vec4), &my_value);
		auto sphere_ptr = static_cast<sphere*>(glMapNamedBufferRange(m_sphere_buffer, 0, m_num_spheres * sizeof(sphere), GL_MAP_WRITE_BIT));
		for (int i{ 0 }; i < m_num_spheres; ++i)
		{
			sphere_ptr[i].radius = 0.3;
			sphere_ptr[i].center = glm::vec4{ -3 + i * 2, 0, -15, 0 };
			sphere_ptr[i].color = m_sphere_colors[i];
		}
		glUnmapNamedBuffer(m_sphere_buffer);

		auto light_ptr = static_cast<light*>(glMapNamedBufferRange(m_light_buffer, 0, m_num_lights * sizeof(light), GL_MAP_WRITE_BIT));
		for (int i{ 0 }; i < m_num_lights; ++i)
		{
			light_ptr[i].center = glm::vec4{ i, 2, -15, 0 };
		}
		glUnmapNamedBuffer(m_light_buffer);

		auto plane_ptr = static_cast<plane*>(glMapNamedBufferRange(m_plane_buffer, 0, m_num_planes * sizeof(plane), GL_MAP_WRITE_BIT));
		plane_ptr[0].normal = glm::vec4(0.0f, 0.0f, -20.0f, 0);
		plane_ptr[0].d = 30.0f;

		plane_ptr[1].normal = glm::vec4(0.0f, 0.0f, 20.0f, 0);
		plane_ptr[1].d = 30.0f;

		plane_ptr[2].normal = glm::vec4(-20.0f, 0.0f, 0.0f, 0);
		plane_ptr[2].d = 30.0f;

		plane_ptr[3].normal = glm::vec4(20.0f, 0.0f, 0.0f, 0);
		plane_ptr[3].d = 30.0f;

		plane_ptr[4].normal = glm::vec4(0.0f, -20.0f, 0.0f, 0);
		plane_ptr[4].d = 30.0f;

		plane_ptr[5].normal = glm::vec4(0.0f, 20.0f, 0.0f, 0);
		plane_ptr[5].d = 30.0f;
		glUnmapNamedBuffer(m_plane_buffer);

		// Setup draw
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		m_prepare_program->use();
		m_prepare_program->uniform("ray_origin", m_view_position);
		m_prepare_program->uniform("ray_lookat", view_matrix);
		m_prepare_program->uniform("aspect", static_cast<float>(m_info.window_height / m_info.window_width));

		glBindFramebuffer(GL_FRAMEBUFFER, m_ray_fbos[0]);
		static const GLenum draw_buffers[6]{
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4,
			GL_COLOR_ATTACHMENT5
		};
		glDrawBuffers(6, draw_buffers);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Trace draw
		m_trace_program->use();
		recurse(0);

		// Blit draw
		m_blit_program->use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glBindTextureUnit(0, m_tex_composite);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	};

	void recurse(int depth) {
		glBindFramebuffer(GL_FRAMEBUFFER, m_ray_fbos[depth + 1]);

		static const GLenum draw_buffers[6]{
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4,
			GL_COLOR_ATTACHMENT5
		};
		glDrawBuffers(6, draw_buffers);

		glEnablei(GL_BLEND, 0);
		glBlendFunci(0, GL_ONE, GL_ONE);

		glBindTextureUnit(0, m_tex_position[depth]);
		glBindTextureUnit(1, m_tex_reflected[depth]);
		glBindTextureUnit(2, m_tex_reflection_intensity[depth]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		if (depth != (m_max_depth - 1))
		{
			recurse(depth + 1);
		}

		glDisablei(GL_BLEND, 0);
	}
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new RayTracer };
	app->run();
}
