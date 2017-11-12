#pragma once

#include <string>

#include "glad/glad.h"
#include "glm/glm/glm.hpp"

class ComputeShader
{
public:
	ComputeShader() {};
	ComputeShader(const char* path);

	void use();
	void set_bool(const std::string &name, GLboolean value) const;
	void set_int(const std::string &name, GLuint value) const;
	void set_float(const std::string &name, GLfloat value) const;
	void set_vec2(const std::string &name, glm::vec2 vec2) const;
	void set_vec3(const std::string &name, glm::vec3 vec3) const;
	void set_vec4(const std::string &name, glm::vec4 vec4) const;
	void set_mat4(const std::string &name, glm::mat4 value) const;
	void set_mat3(const std::string &name, glm::mat3 value) const;

private:
	GLuint m_id;
	void check_compile_errors(unsigned int ComputeShader, std::string type);
};