#pragma once

#include <string>

#include "glad/glad.h"
#include "glm/glm/glm.hpp"

class GlslProgram
{
public:
	void use();
	void set_bool(const std::string &name, GLboolean value) const;
	void set_int(const std::string &name, GLuint value) const;
	void set_float(const std::string &name, GLfloat value) const;
	void set_vec2(const std::string &name, glm::vec2 vec2) const;
	void set_vec3(const std::string &name, glm::vec3 vec3) const;
	void set_vec4(const std::string &name, glm::vec4 vec4) const;
	void set_mat4(const std::string &name, glm::mat4 value) const;
	void set_mat3(const std::string &name, glm::mat3 value) const;

	class Format {
	public:
		Format& vertex(const char* shader_path);
		Format& tess_control(const char* shader_path);
		Format& tess_eval(const char* shader_path);
		Format& geometry(const char* shader_path);
		Format& fragment(const char* shader_path);
		Format& compute(const char* shader_path);

		std::string m_vertex_shader{};
		std::string m_tess_control_shader{};
		std::string m_tess_eval_shader{};
		std::string m_geometry_shader{};
		std::string m_fragment_shader{};
		std::string m_compute_shader{};

	private:
		std::string load_shader(const char* shader_path);
	};
	
	GlslProgram() {};
	GlslProgram(const Format& format);

private:
	GLuint m_id;
	GLuint compile_shader(std::string shader_string, GLenum program_or_shader_type);
	void check_compile_errors(GLuint shader, GLenum type);
};