// Much taken from https://learnopengl.com

#pragma once

#include "glad/glad.h"
#include "glm/glm/glm.hpp"

// Default initialization values
const GLfloat initial_yaw{ -90.0f };
const GLfloat initial_pitch{ 0.0f };
const GLfloat initial_speed{ 2.5f };
const GLfloat initial_sensitivity{ 0.1f };
const GLfloat minimum_pitch{ -89.0f };
const GLfloat maximum_pitch{ 89.0f };
const GLfloat minimum_zoom{ 1.0f };
const GLfloat maximum_zoom{ 45.0f };
const glm::vec3 initial_up{ 0.0f, 1.0f, 0.0f };
const glm::vec3 initial_origin{ 0.0f, 0.0f, 0.0f };
const glm::vec3 initial_front{ 0.0f, 0.0f, -1.0f };

// Used as an abstraction of window-system specific input methods
enum class Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	void set_position(GLfloat x, GLfloat y, GLfloat z);
	void process_keyboard(Camera_Movement direction, GLfloat delta_time);
	void process_mouse_movement(GLfloat x_offset, GLfloat y_offset, GLboolean constrain_pitch = true);
	void process_mouse_scroll(GLfloat y_offset);
	glm::mat4 get_view_matrix() const;
	glm::mat4 get_proj_matrix() const;
	glm::vec3 get_pos() const;
	glm::vec3 get_front() const;

	Camera(const glm::vec3& position = initial_origin, const glm::vec3& front = initial_front, const glm::vec3& up = initial_up, GLfloat yaw = initial_yaw, GLfloat pitch = initial_pitch, GLfloat speed = initial_speed, GLfloat zoom = maximum_zoom, GLfloat sensitivity = initial_sensitivity);
	Camera(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat up_x = initial_up.x, GLfloat up_y = initial_up.y, GLfloat up_z = initial_up.z, GLfloat yaw = initial_yaw, GLfloat pitch = initial_pitch);

private:
	GLfloat m_yaw;
	GLfloat m_pitch;
	GLfloat m_movement_speed;
	GLfloat m_zoom;
	GLfloat m_mouse_sensitivity;
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_world_up;
	const GLfloat m_near_plane{ 0.1f };
	const GLfloat m_far_plane{ 1000.0f };
	void update_camera_vectors();
};
