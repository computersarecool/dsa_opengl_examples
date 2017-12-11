#include "glm/glm/gtc/matrix_transform.hpp"

#include "camera.h"

// Camera public
void Camera::set_position(const GLfloat x, const GLfloat y, const GLfloat z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;

	update_camera_vectors();
}

void Camera::process_keyboard(Camera_Movement direction, GLfloat delta_time)
{
	GLfloat velocity = m_movement_speed * delta_time;

	switch (direction) {
	case Camera_Movement::FORWARD:
		m_position += m_front * velocity;
		break;
	case Camera_Movement::BACKWARD:
		m_position -= m_front * velocity;
		break;
	case Camera_Movement::LEFT:
		m_position -= m_right * velocity;
		break;
	case Camera_Movement::RIGHT:
		m_position += m_right * velocity;
		break;
	}
}

void Camera::process_mouse_movement(GLfloat x_offset, GLfloat y_offset, const GLboolean constrain_pitch)
{
	x_offset *= m_mouse_sensitivity;
	y_offset *= m_mouse_sensitivity;

	m_yaw += x_offset;
	m_pitch += y_offset;

	if (constrain_pitch)
	{
		if (m_pitch > maximum_pitch)
			m_pitch = maximum_pitch;
		else if (m_pitch < minimum_pitch)
			m_pitch = minimum_pitch;
	}

	update_camera_vectors();
}

void Camera::process_mouse_scroll(const GLfloat y_offset)
{
	if (m_zoom >= minimum_zoom && m_zoom <= maximum_zoom)
		m_zoom -= y_offset;
	else if (m_zoom <= minimum_zoom)
		m_zoom = minimum_zoom;
	else if (m_zoom >= maximum_zoom)
		m_zoom = maximum_zoom;
}

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, GLfloat yaw, GLfloat pitch, GLfloat speed, GLfloat zoom, GLfloat sensitivity) :
	m_position{ position }, m_front{ front }, m_world_up{ up }, m_yaw{ yaw }, m_pitch{ pitch }, m_movement_speed{ speed }, m_zoom{ zoom }, m_mouse_sensitivity{ sensitivity }
{
	update_camera_vectors();
}

Camera::Camera(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat up_x, GLfloat up_y, GLfloat up_z, GLfloat yaw, GLfloat pitch) :
	m_position{ glm::vec3{pos_x, pos_y, pos_z} }, m_world_up{ glm::vec3{up_x, up_y, up_z} }, m_yaw{ yaw }, m_pitch{ pitch }
{
	update_camera_vectors();
}

glm::mat4 Camera::get_view_matrix() const
{
	return glm::lookAt(m_position, m_position + m_front, m_world_up);
}

glm::mat4 Camera::get_proj_matrix() const
{
	return glm::perspective(glm::radians(m_zoom), 800.0f / 800.0f, m_near_plane, m_far_plane);
}

glm::vec3 Camera::get_pos() const
{
	return glm::vec3(m_position);
}

glm::vec3 Camera::get_front() const
{
	return glm::vec3(m_front);
}

void Camera::update_camera_vectors()
{
	glm::vec3 front_vector{ 1.0f };
	front_vector.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front_vector.y = sin(glm::radians(m_pitch));
	front_vector.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front_vector);

	// Normalize the vectors because their length gets closer to 0 the more you look up or down which results in slower movement
	m_right = glm::normalize(glm::cross(m_front, m_world_up));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
