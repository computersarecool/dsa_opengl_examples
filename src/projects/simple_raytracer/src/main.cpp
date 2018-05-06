// A raytracer
// TODO: Make fullscreen quads separable programs

#include <memory>
#include <vector>
#include <iostream>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

class RayTracer : public Application
{
private:
	// UBO structs
	// These use `alignas` to avoid padding
	struct alignas(16) sphere
	{
		glm::vec4 center;
		glm::vec4 color;
		float radius;
	};

	struct light
	{
		glm::vec4 center;
	};

	struct alignas(16) plane
	{
        glm::vec4 normal;
        glm::vec4 center;
        glm::vec4 color;
	};

    // Spheres (positions, colors, radii)
    std::vector<sphere> m_spheres{
        {
            glm::vec4{ 0 },
            glm::vec4{ 0.8f, 0.5f, 0.5f, 0 },
            0.5f
        },
        {
            glm::vec4{ 0 },
            glm::vec4{0.8f, 0.8f, 0.8f, 0},
            0.75f
        },
        {
            glm::vec4{ 0 },
            glm::vec4{ 0.3f, 0.3f, 0.8f, 0 },
            0.25f
        }
    };

    // Lights (positions, ignoring the fourth component)
    std::vector<glm::vec4> m_lights{
        glm::vec4{ -2, 2, -1, 0 },
        glm::vec4{ 2, -2, 1, 0 }
    };

    // Planes (normals, positions and colors, ignoring the fourth component)
    std::vector<plane> m_planes{
        // Left plane
        {
            glm::vec4{ 1, 0, 0, 0 },
            glm::vec4{ -3, 0, 0, 0 },
            glm::vec4{ 1, 0.4f, 0.4f, 0 }
        },
        // Right plane
        {
            glm::vec4{ -1, 0, 0, 0 },
            glm::vec4{ 3, 0, 0, 0 },
            glm::vec4{ 0.3f, 0.8f, 0.8f, 0 }
        },
        // Bottom plane
        {
            glm::vec4{ 0, 1, 0, 0 },
            glm::vec4{ 0, -2, 0, 0 },
            glm::vec4{ 0.9f, 1, 0.9f, 0 }
        },
        // Top plane
        {
            glm::vec4{ 0, -1, 0, 0 },
            glm::vec4{ 0, 2, 0, 0 },
            glm::vec4{ 0.5, 0, 0.5, 0 }

        },
        // Back plane
        {
            glm::vec4{ 0, 0, 1, 0 },
            glm::vec4{ 0, 0, -10, 0 },
            glm::vec4{ 1, 0.9f, 0.4, 0 }
        },
        // Front plane
        {
            glm::vec4{ 0, 0, -1, 0 },
            glm::vec4{ 0, 0, 10, 0 },
            glm::vec4{ 0.7f, 0, 0, 0 }
        }
    };

    // Constants
    static const int m_max_recursion_depth{ 5 };

    // UBO binding indices
	const GLuint m_sphere_ubo_binding_index{ 0 };
    const GLuint m_plane_ubo_binding_index{ 1 };
    const GLuint m_light_ubo_binding_index{ 2 };

	// UBO handles
	GLuint m_light_buffer;
	GLuint m_sphere_buffer;
	GLuint m_plane_buffer;

	// FBO texture handles
	GLuint m_composite_texture;
	GLuint m_ray_origin_texture[m_max_recursion_depth];
	GLuint m_ray_direction_texture[m_max_recursion_depth];
	GLuint m_ray_reflection_color_texture[m_max_recursion_depth];

	// GLSL Programs
	std::unique_ptr<GlslProgram> m_prepare_program;
	std::unique_ptr<GlslProgram> m_trace_program;
    std::unique_ptr<GlslProgram> m_blit_program;

    // Camera constants
    glm::vec3 m_view_position{ glm::vec3{ 0, 0, 5 } };
    Camera m_camera{ m_view_position };

	// Other OpenGL objects
	GLuint m_vao;
    GLuint m_sampler;
    GLsync m_sync_object;
    sphere* m_sphere_ptr;

    // Make sure there is an FBO for every recursion level
    std::vector<GLuint> m_ray_fbos { std::vector<GLuint>(m_max_recursion_depth) };
    std::vector<GLenum> m_fbo_draw_buffers{
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };

    void lock_buffer()
    {
        if (m_sync_object)
        {
            glDeleteSync(m_sync_object);
        }

        m_sync_object = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

	virtual void set_info() override
	{
		Application::set_info();
		m_info.title = "Raytracer example";
	}

	virtual void setup() override
	{
		// Load shaders
		m_prepare_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/trace_prepare.vert").fragment("../assets/shaders/trace_prepare.frag") });
		m_trace_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/raytracer.vert").fragment("../assets/shaders/raytracer.frag") });
		m_blit_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/blit.vert").fragment("../assets/shaders/blit.frag") });

		// Create buffers and bind UBOs
		glCreateBuffers(1, &m_sphere_buffer);
		glNamedBufferStorage(m_sphere_buffer, m_spheres.size() * sizeof(sphere), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_sphere_ubo_binding_index, m_sphere_buffer, 0, m_spheres.size() * sizeof(sphere));

		glCreateBuffers(1, &m_light_buffer);
		glNamedBufferStorage(m_light_buffer, m_lights.size() * sizeof(light), nullptr, GL_MAP_WRITE_BIT);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_light_ubo_binding_index, m_light_buffer, 0, m_lights.size() * sizeof(light));

		glCreateBuffers(1, &m_plane_buffer);
		glNamedBufferStorage(m_plane_buffer, m_planes.size() * sizeof(plane), nullptr, GL_MAP_WRITE_BIT);
		glBindBufferRange(GL_UNIFORM_BUFFER, m_plane_ubo_binding_index, m_plane_buffer, 0, m_planes.size() * sizeof(plane));

        // Upload data that does not change to UBOs
        // An alternative to mapping a buffer could be using glNamedBufferSubData
        // i.e. glNamedBufferSubData(m_sphere_buffer, 0, sizeof(glm::vec4), &my_value);
        auto plane_ptr = static_cast<plane*>(glMapNamedBufferRange(m_plane_buffer, 0, m_planes.size() * sizeof(plane), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT));
        memcpy(plane_ptr, m_planes.data(), m_planes.size() * sizeof(plane));
        glUnmapNamedBuffer(m_plane_buffer);

        auto light_ptr = static_cast<light*>(glMapNamedBufferRange(m_light_buffer, 0, m_lights.size() * sizeof(light), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT));
        memcpy(light_ptr, m_lights.data(), m_lights.size() * sizeof(light));
        glUnmapNamedBuffer(m_light_buffer);

        // The sphere data is updated every frame so its buffer is persistently mapped
        m_sphere_ptr = static_cast<sphere*>(glMapNamedBufferRange(m_sphere_buffer, 0, m_spheres.size() * sizeof(sphere), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_INVALIDATE_RANGE_BIT));

		// Create and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// Create FBOs
		glCreateFramebuffers(static_cast<GLsizei>(m_max_recursion_depth), m_ray_fbos.data());

        // Create a sampler
        // This is used by all three of our samplers
        glCreateSamplers(1, &m_sampler);
        glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        for (GLuint index{ 0 }; index < 3; ++index)
        {
            glBindSampler(index, m_sampler);
        }

		// Create the composite texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_composite_texture);
		glTextureStorage2D(m_composite_texture, 1, GL_RGB16F, m_info.window_width, m_info.window_height);

        // Create the other FBO textures
		glCreateTextures(GL_TEXTURE_2D, m_max_recursion_depth, m_ray_origin_texture);
		glCreateTextures(GL_TEXTURE_2D, m_max_recursion_depth, m_ray_direction_texture);
		glCreateTextures(GL_TEXTURE_2D, m_max_recursion_depth, m_ray_reflection_color_texture);

        // Set up the FBOs' textures' stores and the FBO draw buffers
        for (int index{ 0 }; index < m_max_recursion_depth; ++index)
		{
            // Set up FBOs' textures' stores
            glTextureStorage2D(m_ray_origin_texture[index], 1, GL_RGB32F, m_info.window_width, m_info.window_height);
            glTextureStorage2D(m_ray_direction_texture[index], 1, GL_RGB16F, m_info.window_width, m_info.window_height);
            glTextureStorage2D(m_ray_reflection_color_texture[index], 1, GL_RGB16F, m_info.window_width, m_info.window_height);

            // Set FBOs framebuffer textures
            glNamedFramebufferTexture(m_ray_fbos[index], GL_COLOR_ATTACHMENT0, m_composite_texture, 0);
			glNamedFramebufferTexture(m_ray_fbos[index], GL_COLOR_ATTACHMENT1, m_ray_origin_texture[index], 0);
			glNamedFramebufferTexture(m_ray_fbos[index], GL_COLOR_ATTACHMENT2, m_ray_direction_texture[index], 0);
			glNamedFramebufferTexture(m_ray_fbos[index], GL_COLOR_ATTACHMENT3, m_ray_reflection_color_texture[index], 0);

            // Set FBOs draw buffers
            glNamedFramebufferDrawBuffers(m_ray_fbos[index], static_cast<GLsizei>(m_fbo_draw_buffers.size()), m_fbo_draw_buffers.data());
        }
	}

	virtual void render(double current_time) override
	{
        // Wait until GPU is done with buffer
        glClientWaitSync(m_sync_object, GL_SYNC_FLUSH_COMMANDS_BIT, 1);

        // Update sphere position data
        // This could be done with a uniform
		auto sphere_x_offset { static_cast<float>(cos(current_time)) / 2.0f };
        for (int index{ 0 }; index < m_spheres.size(); ++index)
		{
            float sphere_x { static_cast<float>(index) / m_spheres.size() * 4.5f - 1.25f };

            m_sphere_ptr[index].center = glm::vec4{ sphere_x + sphere_x_offset, -1, -5, 0 };
            m_sphere_ptr[index].color = m_spheres[index].color;
            m_sphere_ptr[index].radius = m_spheres[index].radius;
		}
        glUnmapNamedBuffer(m_sphere_buffer);

        // Set viewport dimensions
		glViewport(0, 0, m_info.window_width, m_info.window_height);

        // Setup draw
        m_prepare_program->use();
		m_prepare_program->uniform("ray_origin", m_view_position);
		m_prepare_program->uniform("ray_lookat", m_camera.get_view_matrix());
        glBindFramebuffer(GL_FRAMEBUFFER, m_ray_fbos[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Trace draw
		m_trace_program->use();
        recurse(0);

        // Blit draw
		m_blit_program->use();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTextureUnit(0, m_composite_texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        lock_buffer();
	};

	void recurse(int depth) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ray_fbos[depth + 1]);

        // Enable additive blending
        glEnablei(GL_BLEND, 0);
		glBlendFunci(0, GL_ONE, GL_ONE);

		glBindTextureUnit(0, m_ray_origin_texture[depth]);
		glBindTextureUnit(1, m_ray_direction_texture[depth]);
		glBindTextureUnit(2, m_ray_reflection_color_texture[depth]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		if (depth != (m_max_recursion_depth - 1))
		{
			recurse(depth + 1);
		}

        // Do not blend the blit operation
		glDisablei(GL_BLEND, 0);
	}
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new RayTracer };
	app->run();
}
