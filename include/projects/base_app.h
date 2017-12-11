#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

class Application
{
public:
	Application() {}
	virtual ~Application() {}
	virtual void run() final;

protected:	
	struct AppInfo
	{
		char* title;
		int window_width;
		int window_height;
		int major_version;
		int minor_version;
		int samples;
		int cursor;
		bool resizeable;
	} m_info;

	GLFWwindow* m_window;

	virtual void set_info();
	virtual void init() final;
	virtual void setup() {};
	virtual void draw() final;
	virtual void render(double current_time) {}
	virtual void shutdown() {}
	virtual void on_key(int key, int action);
	virtual void on_resize(int width, int height) {}
	virtual void on_mouse_button(int button, int action) {}
	virtual void on_mouse_move(double x_pos, double y_pos) {}
	virtual void on_mouse_wheel(double x_offset, double y_offset) {}
	static void _check_gl_error(const char* file, int line);
	#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
};
