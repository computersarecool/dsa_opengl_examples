#include "glm/glm/gtc/matrix_transform.hpp"

#include "camera.h"

// Camera public
void Camera::set_position(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;

	update_camera_vectors();
}

void Camera::process_keyboard(Camera_Movement direction, float delta_time)
{
	float velocity = m_movement_speed * delta_time;

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

void Camera::process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch)
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

void Camera::process_mouse_scroll(float y_offset)
{
	if (m_zoom >= minimum_zoom && m_zoom <= maximum_zoom)
		m_zoom -= y_offset;

	else if (m_zoom <= minimum_zoom)
		m_zoom = minimum_zoom;
	
	else if (m_zoom >= maximum_zoom)
		m_zoom = maximum_zoom;
}

Camera::Camera(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up, float yaw, float pitch, float speed, float zoom, float sensitivity) :
	m_position{ position }, m_front{ front }, m_world_up{ up },
    m_yaw{ yaw }, m_pitch{ pitch }, m_movement_speed{ speed },
    m_zoom{ zoom }, m_mouse_sensitivity{ sensitivity },
    m_far_plane{initial_far_plane}, m_near_plane{initial_near_plane}
{
	update_camera_vectors();
}

Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) :
	m_position{ glm::vec3{pos_x, pos_y, pos_z} },
    m_world_up{ glm::vec3{up_x, up_y, up_z} },
    m_yaw{ yaw }, m_pitch{ pitch },
    m_far_plane{initial_far_plane}, m_near_plane{initial_near_plane}

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
	front_vector.x = static_cast<float>(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
	front_vector.y = static_cast<float>(sin(glm::radians(m_pitch)));
	front_vector.z = static_cast<float>(sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));

    // Normalize the vectors because their length gets closer to 0 the more you look up or down which results in slower movement
    m_front = glm::normalize(front_vector);
	m_right = glm::normalize(glm::cross(m_front, m_world_up));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}
