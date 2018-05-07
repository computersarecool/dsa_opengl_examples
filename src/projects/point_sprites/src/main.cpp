// This is a very simple point sprite example

#include <iostream>
#include <memory>
#include <vector>

#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"
#include "camera.h"

class PointSpriteExample : public Application
{
private:
	GLuint m_vao;
	const int m_num_points{ 36 };
	const std::vector<GLfloat>m_clear_color{ 0.2f, 0.0f, 0.2f, 1.0f };
	Camera m_camera{ glm::vec3{0, 0, 5} };
	std::unique_ptr<GlslProgram> m_shader;

	virtual void set_info() override
	{
		Application::set_info();	
		m_info.title = "Point sprite example";
	}

	virtual void setup() override
	{
        // Create shader
		m_shader.reset(new GlslProgram{ GlslProgram::Format().vertex("../assets/shaders/point_sprite.vert").fragment("../assets/shaders/point_sprite.frag")});
        m_shader->use();

        // Set up and bind VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

        // Set OpenGL State
        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_ONE, GL_ONE);
	}

	virtual void render(double current_time) override
	{
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color.data());
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

        // Set uniforms and draw
		m_shader->uniform("u_projection_matrix", m_camera.get_proj_matrix());
        for (int index { 0 }; index != m_num_points; index++)
        {
			// This should use a random engine
            const float x_offset = static_cast<float>((static_cast<double>(rand()) / (RAND_MAX) * 2.0) - 1.0);
            const float y_offset = static_cast<float>((static_cast<double>(rand()) / (RAND_MAX) * 2.0) - 1.0);
            m_shader->uniform("u_x_offset", x_offset);
            m_shader->uniform("u_y_offset", y_offset);
            glDrawArrays(GL_POINTS, 0, 1);
        }
	};
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new PointSpriteExample};
	app->run();
}
