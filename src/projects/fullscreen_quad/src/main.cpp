// This renders a full screen quad
#include <memory>
#include <vector>

#include "base_app.h"
#include "glsl_program.h"

class FullscreenQuadExample : public Application
{
private:
    GLuint m_vao { 0 };
    const GLuint m_num_vertices{ 4 };
    std::unique_ptr<GlslProgram> m_shader;

    void setup() override
    {
        // Create shader
        m_shader = std::make_unique<GlslProgram>(GlslProgram::Format().vertex("../assets/shaders/quad.vert").fragment("../assets/shaders/quad.frag"));
        m_shader->use();
        m_shader->introspect();

        // Setup and bind a VAO
        glCreateVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
    }

    void render(double current_time) override
    {
        m_shader->uniform("time", static_cast<float>(current_time));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_num_vertices);
    };
};

int main(int argc, char* argv[])
{
    std::unique_ptr<Application> app{ new FullscreenQuadExample };
    app->run();
}