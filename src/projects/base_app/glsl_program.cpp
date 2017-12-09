#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/glm/gtc/type_ptr.hpp"

#include "glsl_program.h"

void GlslProgram::use()
{
	glUseProgram(m_id);
}

void GlslProgram::set_bool(const std::string &name, GLboolean value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), static_cast<GLuint>(value));
}

void GlslProgram::set_int(const std::string &name, GLuint value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void GlslProgram::set_float(const std::string &name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void GlslProgram::set_vec2(const std::string &name, glm::vec2 vec2) const
{
	glUniform2f(glGetUniformLocation(m_id, name.c_str()), vec2.x, vec2.y);
}

void GlslProgram::set_vec3(const std::string &name, glm::vec3 vec3) const
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), vec3.x, vec3.y, vec3.z);
}

void GlslProgram::set_vec4(const std::string &name, glm::vec4 vec4) const
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), vec4.x, vec4.y, vec4.z, vec4.w);
}

void GlslProgram::set_mat3(const std::string &name, glm::mat3 value) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void GlslProgram::set_mat4(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

// GlslProgram::Format Public
GlslProgram::Format& GlslProgram::Format::vertex(const char* shader_path)
{
	m_vertex_shader = load_shader(shader_path);
	return *this;
}

GlslProgram::Format& GlslProgram::Format::tess_control(const char* shader_path)
{
	m_tess_control_shader = load_shader(shader_path);
	return *this;
}

GlslProgram::Format& GlslProgram::Format::tess_eval(const char* shader_path)
{
	m_tess_eval_shader = load_shader(shader_path);
	return *this;
}

GlslProgram::Format& GlslProgram::Format::geometry(const char* shader_path)
{
	m_geometry_shader = load_shader(shader_path);
	return *this;
}

GlslProgram::Format& GlslProgram::Format::fragment(const char* shader_path)
{
	m_fragment_shader = load_shader(shader_path);
	return *this;
}

GlslProgram::Format& GlslProgram::Format::compute(const char* shader_path)
{
	m_compute_shader = load_shader(shader_path);
	return *this;
}

// GlslProgram::Format Private
std::string GlslProgram::Format::load_shader(const char* shader_path)
{
	std::ifstream shader_file;
	shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Convert file contents into a std::string
		shader_file.open(shader_path);
		std::stringstream shader_string_stream;
		shader_string_stream << shader_file.rdbuf();
		shader_file.close();
		return shader_string_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ::PATH" << shader_path << std::endl;
		throw;
	}
}

GlslProgram::GlslProgram(const Format& format)
{
	m_id = glCreateProgram();
	GLuint vertex_shader_handle = compile_shader(format.m_vertex_shader, GL_VERTEX_SHADER);
	GLuint tess_control_shader_handle = compile_shader(format.m_tess_control_shader, GL_TESS_CONTROL_SHADER);
	GLuint tess_eval_shader_handle = compile_shader(format.m_tess_eval_shader, GL_TESS_EVALUATION_SHADER);
	GLuint geometry_shader_handle = compile_shader(format.m_geometry_shader, GL_GEOMETRY_SHADER);
	GLuint fragment_shader_handle = compile_shader(format.m_fragment_shader, GL_FRAGMENT_SHADER);
	GLuint compute_shader_handle = compile_shader(format.m_compute_shader, GL_COMPUTE_SHADER);

	if (vertex_shader_handle)
	{
		glAttachShader(m_id, vertex_shader_handle);
	}
	if (tess_control_shader_handle)
	{
		glAttachShader(m_id, tess_control_shader_handle);
	}
	if (tess_eval_shader_handle)
	{
		glAttachShader(m_id, tess_eval_shader_handle);
	}
	if (geometry_shader_handle)
	{
		glAttachShader(m_id, geometry_shader_handle);
	}
	if (fragment_shader_handle)
	{
		glAttachShader(m_id, fragment_shader_handle);
	}
	if (compute_shader_handle)
	{
		glAttachShader(m_id, compute_shader_handle);
	}
	
	glLinkProgram(m_id);
	check_compile_errors(m_id, GL_SHADER);

	if (vertex_shader_handle)
	{
		glDetachShader(m_id, vertex_shader_handle);
		glDeleteShader(vertex_shader_handle);
	}
	if (tess_control_shader_handle)
	{
		glDetachShader(m_id, tess_control_shader_handle);
		glDeleteShader(tess_control_shader_handle);
	}
	if (tess_eval_shader_handle)
	{
		glDetachShader(m_id, tess_eval_shader_handle);
		glDeleteShader(vertex_shader_handle);
	}
	if (geometry_shader_handle)
	{
		glDetachShader(m_id, vertex_shader_handle);
		glDeleteShader(vertex_shader_handle);
	}
	if (fragment_shader_handle)
	{
		glDetachShader(m_id, vertex_shader_handle);
		glDeleteShader(vertex_shader_handle);
	}
	if (compute_shader_handle)
	{
		glDetachShader(m_id, vertex_shader_handle);
		glDeleteShader(vertex_shader_handle);
	}
}

GLuint GlslProgram::compile_shader(std::string shader_string, GLenum shader_type)
{
	GLuint shader_handle{ 0 };

	if (!shader_string.empty())
	{
		const char* shader_c_string = shader_string.c_str();
		shader_handle = glCreateShader(shader_type);
		glShaderSource(shader_handle, 1, &shader_c_string, nullptr);
		glCompileShader(shader_handle);
		GlslProgram::check_compile_errors(shader_handle, shader_type);
	}

	return shader_handle;
}

void GlslProgram::check_compile_errors(GLuint shader, GLenum program_or_shader_type)
{
	GLint success;
	const GLuint log_length{ 1024 };
	GLchar info_log[log_length];

	if (program_or_shader_type == GL_SHADER)
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, log_length, nullptr, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR: " << program_or_shader_type << "\n" << info_log << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, log_length, nullptr, info_log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << program_or_shader_type << "\n" << info_log << std::endl;
		}
	}
}
