#pragma once

#include <string>
#include <vector>

#include "glad/glad.h"
#include "glm/glm/glm.hpp"

class GlslProgram
{
public:
	class Format {
	public:
		Format& vertex(const char* shader_path);
		Format& tess_control(const char* shader_path);
		Format& tess_eval(const char* shader_path);
		Format& geometry(const char* shader_path);
		Format& fragment(const char* shader_path);
		Format& compute(const char* shader_path);

		std::string m_vertex_shader;
		std::string m_tess_control_shader;
		std::string m_tess_eval_shader;
		std::string m_geometry_shader;
		std::string m_fragment_shader;
		std::string m_compute_shader;

	private:
		std::string load_shader(const char* shader_path);
	};

	void use() const;
	void uniform(const GLchar* name, const GLboolean value) const;
	void uniform(const GLchar* name, const GLuint value) const;
	void uniform(const GLchar* name, const GLfloat value) const;
	void uniform(const GLchar* name, const glm::vec2 value) const;
	void uniform(const GLchar* name, const glm::vec3 value) const;
	void uniform(const GLchar* name, const glm::vec4 value) const;
	void uniform(const GLchar* name, const glm::mat3 value) const;
	void uniform(const GLchar* name, const glm::mat4 value) const;

	GlslProgram() {};
	GlslProgram(const Format& format, const bool separable = false);

private:
	GLuint m_handle;
	GLuint compile_shader(const std::string shader_string, const GLenum shader_type);
	void introspect() const;
	void check_compile_errors(const GLuint program_or_shader, const GLenum program_or_shader_type);
};