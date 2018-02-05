// A point sprite example
#include <memory>
#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "glsl_program.h"

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
        // Calculate aspect
        m_aspect = m_info.window_width / m_info.window_height;

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

        // Calculate uniforms
        current_time *= m_time_scale;
        current_time -= floor(current_time);

        m_projection_matrix = glm::perspective(glm::radians(m_fov_in_degrees), m_aspect, m_near_plane, m_far_plane);
        
		// Set uniforms and draw
        m_shader->uniform("uCurrentTime", static_cast<GLfloat>(current_time));
		m_shader->uniform("uProjectionMatrix", m_projection_matrix);
		glDrawArrays(GL_POINTS, 0, m_num_points);
	};

	// Member variables
	std::unique_ptr<GlslProgram> m_shader;
    glm::mat4 m_projection_matrix;
    GLuint m_vao { 0 };
    float m_aspect { 0.0f };
    const float m_near_plane{ 0.1f };
    const float m_far_plane{ 1000.0f };
    const float m_fov_in_degrees{ 50.0f };
    const int m_num_points{ 36 };
    const float m_time_scale = 0.1f;
	const float m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new PointSpriteExample};
	app->run();
}
