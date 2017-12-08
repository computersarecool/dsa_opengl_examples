#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/glm/gtc/type_ptr.hpp"

#include "glsl_program.h"

GLuint GlslProgram::load_shader(const char* shader_path, GLenum shader_type)
{
	std::string shader_string;
	std::ifstream shader_file;
	shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Convert GLSL file into a std::string
		shader_file.open(shader_path);
		std::stringstream shader_string_stream;
		shader_string_stream << shader_file.rdbuf();
		shader_file.close();
		shader_string = shader_string_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ::PATH" << shader_path << std::endl;
		throw;
	}

	const GLchar* shader_code = shader_string.c_str();

	// Compile the shader
	GLuint shader_handle{ 0 };
	shader_handle = glCreateShader(shader_type);
	glShaderSource(shader_handle, 1, &shader_code, nullptr);
	glCompileShader(shader_handle);
	GlslProgram::check_compile_errors(shader_handle, shader_path);
	return shader_handle;
}

GlslProgram::GlslProgram(const char* vertex_shader_path, const char* fragment_shader_path, const char* tess_control_shader_path, const char* tess_eval_shader_path, const char* geom_shader_path)
{
	// Vertex and fragment shaders
	GLuint vertex_shader = load_shader(vertex_shader_path, GL_VERTEX_SHADER);
	GLuint fragment_shader = load_shader(fragment_shader_path, GL_FRAGMENT_SHADER);

	// Tesselation shaders
	GLuint tess_control_shader{ 0 };
	GLuint tess_eval_shader{ 0 };
	if (strlen(tess_control_shader_path))
	{
		tess_control_shader = load_shader(tess_control_shader_path, GL_TESS_CONTROL_SHADER);
		tess_eval_shader = load_shader(tess_eval_shader_path, GL_TESS_EVALUATION_SHADER);
	}

	// Geometry shader
	GLuint geometry_shader{ 0 };
	if (strlen(geom_shader_path))
	{
		tess_control_shader = load_shader(tess_control_shader_path, GL_GEOMETRY_SHADER);
	}

	// Create and link the GLSL program
	m_id = glCreateProgram();
	glAttachShader(m_id, vertex_shader);
	glAttachShader(m_id, fragment_shader);

	if (tess_control_shader)
	{
		glAttachShader(m_id, tess_control_shader);
		glAttachShader(m_id, tess_eval_shader);
	}

	if (geometry_shader)
	{
		glAttachShader(m_id, geometry_shader);
	}

	glLinkProgram(m_id);
	check_compile_errors(m_id, "PROGRAM");

	// Delete the shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteShader(tess_control_shader);
	glDeleteShader(tess_eval_shader);
	glDeleteShader(geometry_shader);
}

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

void GlslProgram::check_compile_errors(unsigned int shader, const char* type_or_path)
{
	GLint success;
	const GLuint log_length{ 1024 };
	GLchar info_log[log_length];

	if (type_or_path == "PROGRAM")
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, log_length, nullptr, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type_or_path << "\n" << info_log << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, log_length, nullptr, info_log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR at path: " << type_or_path << "\n" << info_log << std::endl;
		}
	}
}
