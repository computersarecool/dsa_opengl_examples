#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/gtc/type_ptr.hpp"

#include "shader.h"

const GLuint log_length = 1024;

Shader::Shader(const char* vertex_shader_path, const char* fragment_shader_path, const char* tess_control_shader_path, const char* tess_eval_shader_path, const char* geom_shader_path)
{
	// Retrieve the vertex / fragment source code from file path
	std::string vertex_code;
	std::string fragment_code;
	std::ifstream vertex_shader_file;
	std::ifstream fragment_shader_file;

	// Ensure ifstream objects can throw exceptions
	vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Read file's buffer contents into string streams
		vertex_shader_file.open(vertex_shader_path);
		fragment_shader_file.open(fragment_shader_path);

		std::stringstream vertex_shader_stream;
		std::stringstream fragment_shader_stream;

		vertex_shader_stream << vertex_shader_file.rdbuf();
		fragment_shader_stream << fragment_shader_file.rdbuf();
		vertex_shader_file.close();
		fragment_shader_file.close();

		// Convert stream into string
		vertex_code = vertex_shader_stream.str();
		fragment_code = fragment_shader_stream.str();
	}

	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const GLchar* vertex_shader_code = vertex_code.c_str();
	const GLchar* fragment_shader_code = fragment_code.c_str();

	// Compile vertex and fragment shaders
	GLuint vertex;
	GLuint fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_shader_code, nullptr);
	glCompileShader(vertex);
	check_compile_errors(vertex, "VERTEX");

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_shader_code, nullptr);
	glCompileShader(fragment);
	check_compile_errors(fragment, "FRAGMENT");

	// Tesselation shaders
	GLuint tess_control;
	GLuint tess_eval;

	if (strlen(tess_control_shader_path))
	{
		std::string tess_control_code;
		std::string tess_eval_code;
		std::ifstream tess_control_file;
		std::ifstream tess_eval_file;

		tess_control_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		tess_eval_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			tess_control_file.open(tess_control_shader_path);
			tess_eval_file.open(tess_eval_shader_path);

			std::stringstream tess_controlStream;
			std::stringstream tess_evalStream;

			tess_controlStream << tess_control_file.rdbuf();
			tess_evalStream << tess_eval_file.rdbuf();

			tess_control_file.close();
			tess_eval_file.close();

			tess_control_code = tess_controlStream.str();
			tess_eval_code = tess_evalStream.str();
		}

		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		const GLchar* tcShaderCode = tess_control_code.c_str();
		const GLchar* teShaderCode = tess_eval_code.c_str();

		
		tess_control = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tess_control, 1, &tcShaderCode, NULL);
		glCompileShader(tess_control);
		check_compile_errors(tess_control, "TESSELATION CONTROL");

		tess_eval = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tess_eval, 1, &teShaderCode, NULL);
		glCompileShader(tess_eval);
		check_compile_errors(tess_eval, "TESSELATION EVALUATION");
	}

	// Geometry shader
	GLuint geometry;

	if (strlen(geom_shader_path))
	{
		std::string geom_code;
		std::ifstream geom_file;

		geom_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			std::stringstream geom_stream;
			geom_file.open(geom_shader_path);
			geom_stream << geom_file.rdbuf();
			geom_file.close();
			geom_code = geom_stream.str();

		}

		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		const GLchar* geom_shader_code = geom_code.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geom_shader_code, NULL);
		glCompileShader(geometry);
		check_compile_errors(geometry, "GEOMETRY");
	}

	// Shader program
	m_id = glCreateProgram();
	glAttachShader(m_id, vertex);
	glAttachShader(m_id, fragment);

	if (strlen(tess_control_shader_path))
	{
		glAttachShader(m_id, tess_control);
		glAttachShader(m_id, tess_eval);
	}

	if (strlen(geom_shader_path))
	{
		glAttachShader(m_id, geometry);
	}

	glLinkProgram(m_id);
	check_compile_errors(m_id, "PROGRAM");

	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (strlen(tess_control_shader_path))
	{
		glDeleteShader(tess_control);
		glDeleteShader(tess_eval);
	}

	if (strlen(geom_shader_path))
	{
		glDeleteShader(geometry);
	}
}

void Shader::use()
{
	glUseProgram(m_id);
}

void Shader::set_bool(const std::string &name, GLboolean value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), static_cast<GLuint>(value));
}

void Shader::set_int(const std::string &name, GLuint value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::set_float(const std::string &name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::set_vec2(const std::string &name, glm::vec2 vec2) const
{
	glUniform2f(glGetUniformLocation(m_id, name.c_str()), vec2.x, vec2.y);
}

void Shader::set_vec3(const std::string &name, glm::vec3 vec3) const
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), vec3.x, vec3.y, vec3.z);
}

void Shader::set_vec4(const std::string &name, glm::vec4 vec4) const
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), vec4.x, vec4.y, vec4.z, vec4.w);
}

void Shader::set_mat3(const std::string &name, glm::mat3 value) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_mat4(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::check_compile_errors(unsigned int shader, std::string type)
{
	GLint success;
	GLchar info_log[log_length];

	if (type == "PROGRAM")
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, log_length, nullptr, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << "\n ---- " << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, log_length, nullptr, info_log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << "\n ---- " << std::endl;
		}
	}
}
