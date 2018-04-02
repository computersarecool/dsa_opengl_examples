// Much taken from https://learnopengl.com

#pragma once

#include "glm/glm/glm.hpp"

// Default initialization values
const float initial_yaw{ -90.0f };
const float initial_pitch{ 0.0f };
const float initial_speed{ 2.5f };
const float initial_sensitivity{ 0.1f };
const float minimum_pitch{ -89.0f };
const float maximum_pitch{ 89.0f };
const float minimum_zoom{ 1.0f };
const float maximum_zoom{ 45.0f };
const float initial_near_plane{ 0.1f };
const float initial_far_plane{ 1000.0f };
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
	void set_position(float x, float y, float z);
	void set_position(glm::vec3 position);
	void process_keyboard(Camera_Movement direction, float delta_time);
	void process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch = true);
	void process_mouse_scroll(float y_offset);

	glm::mat4 get_view_matrix() const;
	glm::mat4 get_proj_matrix() const;
	glm::vec3 get_pos() const;
	glm::vec3 get_front() const;

	explicit Camera(const glm::vec3& position = initial_origin, const glm::vec3& front = initial_front, const glm::vec3& up = initial_up, float yaw = initial_yaw, float pitch = initial_pitch, float speed = initial_speed, float zoom = maximum_zoom, float sensitivity = initial_sensitivity);
	Camera(float pos_x, float pos_y, float pos_z, float up_x = initial_up.x, float up_y = initial_up.y, float up_z = initial_up.z, float yaw = initial_yaw, float pitch = initial_pitch);

private:
	float m_yaw;
	float m_pitch;
	float m_movement_speed;
	float m_zoom;
	float m_mouse_sensitivity;
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_right;
    glm::vec3 m_up;
    glm::vec3 m_world_up;
	const float m_near_plane;
	const float m_far_plane;
	void update_camera_vectors();
};
