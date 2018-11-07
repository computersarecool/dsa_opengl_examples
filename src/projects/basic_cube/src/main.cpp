// A rotating basic cube with model-space positions as colors

#include <memory>
#include <vector>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

static const GLfloat cube_vertices[]{
    // Positions          // Normals
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

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
    GLuint m_vao{ 0 };
    GLuint m_vbo{ 0 };
    std::unique_ptr<GlslProgram> m_shader;
    Camera m_camera{ glm::vec3{0, 0, 5} };
    const glm::vec3 m_world_up{ glm::vec3{ 0, 1, 0 } };
    const std::vector<GLfloat> m_clear_color { 0.2f, 0.0f, 0.2f, 1.0f };
    const std::string m_cube_mv_matrix_name= "u_model_view_matrix";
    const std::string m_cube_projection_matrix_name = "u_projection_matrix";

    void set_info() override
    {
        Application::set_info();
        m_info.title = "Basic cube example";
    }

    void setup() override
    {
        // Set and use shader
        m_shader = std::make_unique<GlslProgram>(GlslProgram::Format().vertex("../assets/shaders/cube.vert").fragment("../assets/shaders/cube.frag"));
        m_shader->introspect();
        m_shader->use();

        // Cube vertex attributes
        const GLuint elements_per_face{ 6 };

        // Position attributes
        const GLuint position_index{ 0 };
        const GLuint position_size{ 3 };
        const GLenum position_type{ GL_FLOAT };
        const GLboolean position_normalize{ GL_FALSE };
        const GLuint position_offset_in_buffer{ 0 };

        // Normal attributes
        const GLuint normal_index{ 1 };
        const GLuint normal_size{ 3 };
        const GLenum normal_type{ GL_FLOAT };
        const GLboolean normal_normalize{ GL_FALSE };
        const GLuint normal_offset_in_buffer{ sizeof(GLfloat) * position_size };
        
        // Cube VBO attributes
        const GLuint binding_index{ 0 };
        const GLuint first_element_offset{ 0 };
        const GLuint element_stride{ sizeof(GLfloat) * elements_per_face };

        // Set up the cube VBO
        const GLuint flags{ 0 };
        glCreateBuffers(1, &m_vbo);
        glNamedBufferStorage(m_vbo, sizeof(cube_vertices), cube_vertices, flags);

        // Create and bind cube VAO
        glCreateVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        // Set up position attribute in VAO
        glEnableVertexArrayAttrib(m_vao, position_index);
        glVertexArrayAttribFormat(m_vao, position_index, position_size, position_type, position_normalize, position_offset_in_buffer);
        glVertexArrayAttribBinding(m_vao, position_index, binding_index);

        // Set up normal attribute in VAO
        glEnableVertexArrayAttrib(m_vao, normal_index);
        glVertexArrayAttribFormat(m_vao, normal_index, normal_size, normal_type, normal_normalize, normal_offset_in_buffer);
        glVertexArrayAttribBinding(m_vao, normal_index, binding_index);

        // Set buffer that backs the VAO
        glVertexArrayVertexBuffer(m_vao, binding_index, m_vbo, first_element_offset, element_stride);

        // Set OpenGL State
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void render(double current_time) override
    {
        // Set default framebuffer parameters
        glViewport(0, 0, m_info.window_width, m_info.window_height);
        glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
        glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0, 0);

        // Set uniforms and draw cube
        glm::mat4 model_matrix{ glm::mat4{ 1.0 } };
        model_matrix = glm::rotate(model_matrix, static_cast<float>(current_time), m_world_up);
        m_shader->uniform(m_cube_mv_matrix_name, m_camera.get_view_matrix() * model_matrix);
        m_shader->uniform(m_cube_projection_matrix_name, m_camera.get_proj_matrix());
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices) / sizeof(*cube_vertices));
    };
};

int main(int argc, char* argv[])
{
    std::unique_ptr<Application> app{ new BasicCubeExample };
    app->run();
}
