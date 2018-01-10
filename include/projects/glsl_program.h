#pragma once

#include <string>

#include "glad/glad.h"
#include "glm/glm/glm.hpp"

class GlslProgram
{
public:
	struct Format {
	public:
		std::string m_vertex_shader;
		std::string m_tess_control_shader;
		std::string m_tess_eval_shader;
		std::string m_geometry_shader;
		std::string m_fragment_shader;
		std::string m_compute_shader;

		Format& vertex(const std::string& shader_path);
		Format& tess_control(const std::string& shader_path);
		Format& tess_eval(const std::string& shader_path);
		Format& geometry(const std::string& shader_path);
		Format& fragment(const std::string& shader_path);
		Format& compute(const std::string& shader_path);

	private:
		std::string load_shader(const std::string& shader_path);
	};

	void use() const;
	void uniform(const std::string& name, const GLboolean value) const;
	void uniform(const std::string& name, const GLuint value) const;
	void uniform(const std::string& name, const GLfloat value) const;
	void uniform(const std::string& name, const glm::vec2& value) const;
	void uniform(const std::string& name, const glm::vec3& value) const;
	void uniform(const std::string& name, const glm::vec4& value) const;
	void uniform(const std::string& name, const glm::mat3& value) const;
	void uniform(const std::string& name, const glm::mat4& value) const;

	GlslProgram(const Format& format, bool separable = false);

private:
	GLuint m_handle;
	GLuint compile_shader(const std::string& shader_string, GLenum shader_type) const;
	void check_compile_errors(GLuint program_or_shader, GLenum program_or_shader_type) const;
	void introspect() const;
};