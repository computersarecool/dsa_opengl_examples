#include <iostream>

#include "glad/glad.h"

#include "base_app.h"

// Set window and context parameters
void Application::set_info()
{
	m_info.title = "OpenGL Example";
	m_info.window_width = 800;
	m_info.window_height = 800;
	m_info.major_version = 4;
	m_info.minor_version = 4;
	m_info.samples = 0;
	m_info.resizeable = false;
	m_info.cursor = GLFW_CURSOR_NORMAL;
}

// Initialize GLFW, a window and GLAD
void Application::init()
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		glfwTerminate();
		throw;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_info.major_version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_info.minor_version);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, m_info.resizeable);
	glfwWindowHint(GLFW_SAMPLES, m_info.samples);

	// Create window
	m_window = glfwCreateWindow(m_info.window_width, m_info.window_height, m_info.title, nullptr, nullptr);
	if (!m_window)
	{
		std::cerr << "Failed to open window" << std::endl;
		throw;
	}
	glfwMakeContextCurrent(m_window);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetInputMode(m_window, GLFW_CURSOR, m_info.cursor);

	// Define GLFW Callbacks
	auto glfw_on_resize = [](GLFWwindow* w, int width, int height)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->on_resize(width, height);
	};

	auto glfw_on_key = [](GLFWwindow* w, int key, int scancode, int action, int mods)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->on_key(key, action);
	};

	auto glfw_on_mouse_button = [](GLFWwindow* w, int button, int action, int mods)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->on_mouse_button(button, action);
	};

	auto glfw_on_mouse_move = [](GLFWwindow* w, double x_pos, double y_pos)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->on_mouse_move(x_pos, y_pos);
	};

	auto glfw_on_mouse_wheel = [](GLFWwindow* w, double x_offset, double y_offset)
	{
		static_cast<Application*>(glfwGetWindowUserPointer(w))->on_mouse_wheel(x_offset, y_offset);
	};

	// Set GLFW Callbacks
	glfwSetWindowSizeCallback(m_window, glfw_on_resize);
	glfwSetKeyCallback(m_window, glfw_on_key);
	glfwSetMouseButtonCallback(m_window, glfw_on_mouse_button);
	glfwSetCursorPosCallback(m_window, glfw_on_mouse_move);
	glfwSetScrollCallback(m_window, glfw_on_mouse_wheel);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
}

// Render loop
void Application::draw()
{
	do
	{
		render(glfwGetTime());
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	} while (!(glfwWindowShouldClose(m_window)));
}

// Call what is needed to start the app
void Application::run()
{
	set_info();
	init();
	setup();
	draw();
	shutdown();
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

// Close window when escape is pressed
void Application::on_key(int key, int action)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_window, true);
	}
}

// Function to check OpenGL errors. #defined as check_gl_error
void Application::_check_gl_error(const char* file, int line)
{
	GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
		const char* error;
		switch (err) {
			case GL_INVALID_OPERATION:
				error = "GL_INVALID_OPERATION";
				break;
			case GL_INVALID_ENUM:
				error = "GL_INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:
				error = "GL_INVALID_VALUE";
				break;
			case GL_OUT_OF_MEMORY:
				error = "GL_OUT_OF_MEMORY";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error = "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;
		}
		std::cerr << "Error: " << err << " " << error << " - " << file << ":" << line << std::endl;
	}
}