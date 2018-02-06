// A point sprite example
#include <iostream>
#include <memory>
#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

class PointSpriteExample : public Application
{
private:
	virtual void set_info()
	{
		Application::set_info();	
		m_info.title = "Point sprite example";
	}

	virtual void setup()
	{
        // Create shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/point_sprite.vert").fragment("../assets/shaders/point_sprite.frag")});
        m_shader->use();

        // Set up and bind VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

        // OpenGL State
        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_ONE, GL_ONE);
	}

	virtual void render(double current_time)
	{
		// Set OpenGL state
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

        // Set uniforms and draw
		m_shader->uniform("uProjectionMatrix", m_camera.get_proj_matrix());
        for (int i = 0; i != m_num_points; i++)
        {
            const float x_offset = static_cast<float>((static_cast<double>(rand()) / (RAND_MAX) * 2.0) - 1.0);
            const float y_offset = static_cast<float>((static_cast<double>(rand()) / (RAND_MAX) * 2.0) - 1.0);
            m_shader->uniform("xOffset", x_offset);
            m_shader->uniform("yOffset", y_offset);
            glDrawArrays(GL_POINTS, 0, 1);
        }
	};

	// Member variables
    GLuint m_vao { 0 };
    const int m_num_points{ 36 };
	const float m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
    Camera m_camera{ glm::vec3{0, 0, 5} };
	std::unique_ptr<GlslProgram> m_shader;
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new PointSpriteExample};
	app->run();
}
