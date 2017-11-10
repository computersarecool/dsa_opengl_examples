// A basic point stp
#include "glm/glm/gtc/matrix_transform.hpp"

#include "base_app.h"
#include "shader.h"
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
		m_shader = Shader{ "../assets/shaders/point_sprite.vert", "../assets/shaders/point_sprite.frag" };
    
		// Setup and bind a VAO
		glCreateVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
	}

	virtual void render(double current_time)
	{
		// Set OpenGL state
		glViewport(0, 0, m_info.window_width, m_info.window_height);
		glClearBufferfv(GL_COLOR, 0, m_clear_color);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_ONE, GL_ONE);

        current_time *= m_time_scale;
        current_time -= floor(current_time);
        
        float aspect = m_info.window_width / m_info.window_height;
        m_projection_matrix = glm::perspective(glm::radians(m_fov_in_degrees), aspect, m_near_plane, m_far_plane);
        
		// Set uniforms and draw first cube
		m_shader.use();
        m_shader.set_float("uCurrentTime", current_time);
		m_shader.set_mat4("uProjectionMatrix", m_projection_matrix);
		glDrawArrays(GL_POINTS, 0, m_num_points);
	};

	// Member variables
	Shader m_shader;
	GLuint m_vao;
    glm::mat4 m_projection_matrix;
    const GLfloat m_near_plane{0.1f};
    const GLfloat m_far_plane{1000.0f};
    const GLfloat m_fov_in_degrees{50.0f};
    const GLuint m_num_points{ 36 };
    const GLfloat m_time_scale = 0.1f;
	const GLfloat m_clear_color[4]{ 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
    Application* my_app = new PointSpriteExample;
	my_app->run();
	delete my_app;
}
