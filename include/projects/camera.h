// Much taken from https://learnopengl.com

#pragma once

#include "glm/glm/glm.hpp"

// This enum class is used as an abstraction of window-system specific input methods
enum class Camera_Movement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// These are default arguments for the constructors
namespace {
    float default_yaw{ -90.0f };
    float default_pitch{ 0.0f };
    float default_movement_speed{ 2.5f };
    float default_mouse_sensitivity{ 0.1f };
    float default_maximum_zoom{ 45.0f };
    glm::vec3 default_front{ 0.0f, 0.0f, -1.0f };
    glm::vec3 default_up{ 0.0f, 1.0f, 0.0f };
    glm::vec3 default_position{ 0.0f };

    // These are used in the implementation
    const float default_minimum_pitch{ -89.0f };
    const float default_maximum_pitch{ 89.0f };
    glm::vec3 default_right{ 1.0f, 0.0f, 0.0f };
    glm::vec3 default_world_up{ 0.0f, 1.0f, 0.0f };
    float default_near_plane{ 0.1f };
    float default_far_plane{ 1000.0f };
}

// An abstract camera class that processes input and calculates the corresponding Euler angles, vectors and matrices
class Camera
{
public:
	void process_keyboard(Camera_Movement direction, float delta_time);
	void process_mouse_movement(float x_offset, float y_offset, bool constrain_pitch = true);
	void process_mouse_scroll(float y_offset);

    // Setters
    void set_position(float x, float y, float z);
    void set_position(glm::vec3 position);

    // Getters
	glm::mat4 get_view_matrix() const;
	glm::mat4 get_proj_matrix() const;
	glm::vec3 get_pos() const;
	glm::vec3 get_front() const;

    // Constructors
	explicit Camera(const glm::vec3& position = default_position, const glm::vec3& front = default_front, const glm::vec3& up = default_up, float yaw = default_yaw, float pitch = default_pitch, float speed = default_movement_speed, float zoom = default_maximum_zoom, float sensitivity = default_mouse_sensitivity);
	Camera(float pos_x, float pos_y, float pos_z, float up_x = default_up.x, float up_y = default_up.y, float up_z = default_up.z, float yaw = default_yaw, float pitch = default_pitch);

private:
    const float m_minimum_pitch;
    const float m_maximum_pitch;
    float m_yaw;
    float m_pitch;
    float m_near_plane;
    float m_far_plane;
	float m_movement_speed;
	float m_zoom;
	float m_mouse_sensitivity;
    float m_maximum_zoom;
    float m_minimum_zoom;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_world_up;
    glm::vec3 m_position;

	void update_camera_vectors();
};
