﻿// A raytracer
// TODO: Make fullscreen quad a separable program

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
    // Member variables
    static const int m_max_recursion_depth { 5 };
    const int m_max_framebuffer_width { 2048 };
    const int m_max_framebuffer_height { 1024 };
    const int m_num_spheres{ 128 };
    int m_max_depth { 2 };

    // FBO textures
    GLuint m_tex_composite;
    GLuint m_tex_position[m_max_recursion_depth];
    GLuint m_tex_reflected[m_max_recursion_depth];
    GLuint m_tex_reflection_intensity[m_max_recursion_depth];
    GLuint m_tex_refracted[m_max_recursion_depth];
    GLuint m_tex_refraction_intensity[m_max_recursion_depth];

    // Objects in the scene
    struct sphere
    {
        float radius { 0 };
        glm::vec3 center { 0 };
        glm::vec4 color { 0 };
    };

    struct uniforms_block
    {
        glm::mat4 model_view_matrix;
        glm::mat4 view_matrix;
        glm::mat4 projection_matrix;
    };

    // Buffer objects
    GLuint m_uniforms_buffer;
    GLuint m_sphere_buffer;

    // GLSL Programs
    std::unique_ptr<GlslProgram> m_prepare_program;
    std::unique_ptr<GlslProgram> m_trace_program;
    std::unique_ptr<GlslProgram> m_blit_program;

    // Other OpenGL objects
    std::vector<GLuint> m_ray_fbos {0, 0, 0, 0, 0};
    GLuint m_vao{ 0 };
    const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };

    glm::vec3 m_view_position{ glm::vec3{0, 0, 5} };
    Camera m_camera{ m_view_position };

    virtual void set_info() override
    {
        Application::set_info();
        m_info.title = "Raytracer example";
    }

    virtual void setup() override
    {
        // Load shaders
        m_prepare_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/trace_prepare.vert").fragment("../assets/shaders/trace_prepare.frag")});
        m_trace_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/raytracer.vert").fragment("../assets/shaders/raytracer.frag")});
        m_blit_program.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/blit.vert").fragment("../assets/shaders/blit.frag")});

        // Create buffers
        glCreateBuffers(1, &m_uniforms_buffer);
        glNamedBufferStorage(m_uniforms_buffer, sizeof(uniforms_block), nullptr, GL_MAP_WRITE_BIT);

        glCreateBuffers(1, &m_sphere_buffer);
        glNamedBufferStorage(m_sphere_buffer, m_num_spheres * sizeof(sphere), nullptr, GL_MAP_WRITE_BIT);

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

        // Set FBO textures and texture parameters
        // TODO: Use a sampler
        for (int i { 0 }; i < m_max_recursion_depth; ++i)
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
        glm::mat4 model_matrix = glm::mat4{1.0};
        glm::mat4 view_matrix = m_camera.get_view_matrix();

        auto uniforms_ptr = static_cast<uniforms_block*>(glMapNamedBufferRange(m_uniforms_buffer, 0, sizeof(uniforms_block), GL_MAP_WRITE_BIT));
        uniforms_ptr->model_view_matrix = view_matrix * model_matrix;
        uniforms_ptr->view_matrix = view_matrix;
        uniforms_ptr->projection_matrix = m_camera.get_proj_matrix();
        glUnmapNamedBuffer(m_uniforms_buffer);

        // Set sphere data
        auto sphere_ptr = static_cast<sphere*>(glMapNamedBufferRange(m_sphere_buffer, 0, m_num_spheres * sizeof(sphere), GL_MAP_WRITE_BIT));
        for (int i { 0 }; i < m_num_spheres; ++i)
        {
            sphere_ptr[i].center = glm::vec3(0, 0, -50);
            //sphere_ptr[i].color = glm::vec4(1.0f, 1.0f, normalize(1.0f, 1.0f);
        }
        glUnmapNamedBuffer(m_sphere_buffer);

        // Set up draw
        glViewport(0, 0, m_info.window_width, m_info.window_height);
        m_prepare_program->use();
        m_prepare_program->uniform("ray_origin", m_view_position);
        m_prepare_program->uniform("ray_lookat", view_matrix);
        m_prepare_program->uniform("aspect", static_cast<float>(m_info.window_height / m_info.window_width));

        glBindFramebuffer(GL_FRAMEBUFFER, m_ray_fbos[0]);
        static const GLenum draw_buffers[6] {
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

    void recurse(int depth)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_ray_fbos[depth + 1]);

        static const GLenum draw_buffers[6] {
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
