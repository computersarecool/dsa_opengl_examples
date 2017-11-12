#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/glm/gtc/type_ptr.hpp"

#include "compute_shader.h"


ComputeShader::ComputeShader(const GLchar* path)
{
	// Retrieve the source code from file path
	std::string shader_code;
	std::ifstream shader_file;

	// Ensure ifstream objects can throw exceptions
	shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Read file's buffer contents into string stream
		shader_file.open(path);
		std::stringstream shader_stream;
		shader_stream << shader_file.rdbuf();
		shader_file.close();

		// Convert stream into a string
		shader_code = shader_stream.str();
	}

	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::ComputeShader::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const GLchar* compute_shader_code = shader_code.c_str();


	// Compile shader and fragment ComputeShaders
	GLuint shader;
	shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &compute_shader_code, nullptr);
	glCompileShader(shader);
	check_compile_errors(shader, "Compute Shader");
	
	// Delete the shader as it is linked into our program now and no longer necessery
	glDeleteShader(shader);

}

void ComputeShader::use()
{
	glUseProgram(m_id);
}

void ComputeShader::set_bool(const std::string &name, GLboolean value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), static_cast<GLuint>(value));
}

void ComputeShader::set_int(const std::string &name, GLuint value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void ComputeShader::set_float(const std::string &name, GLfloat value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void ComputeShader::set_vec2(const std::string &name, glm::vec2 vec2) const
{
	glUniform2f(glGetUniformLocation(m_id, name.c_str()), vec2.x, vec2.y);
}

void ComputeShader::set_vec3(const std::string &name, glm::vec3 vec3) const
{
	glUniform3f(glGetUniformLocation(m_id, name.c_str()), vec3.x, vec3.y, vec3.z);
}

void ComputeShader::set_vec4(const std::string &name, glm::vec4 vec4) const
{
	glUniform4f(glGetUniformLocation(m_id, name.c_str()), vec4.x, vec4.y, vec4.z, vec4.w);
}

void ComputeShader::set_mat3(const std::string &name, glm::mat3 value) const
{
	glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void ComputeShader::set_mat4(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

GLvoid ComputeShader::check_compile_errors(GLuint ComputeShader, std::string type)
{
	const GLint log_length{ 1024 };
	GLchar info_log[log_length];
	GLint success;

	if (type == "PROGRAM")
	{
		glGetProgramiv(ComputeShader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ComputeShader, log_length, nullptr, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << "\n ---- " << std::endl;
		}
	}
	else
	{
		glGetShaderiv(ComputeShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(ComputeShader, log_length, nullptr, info_log);
			std::cout << "ERROR::ComputeShader_COMPILATION_ERROR of type: " << type << "\n" << info_log << "\n ---- " << std::endl;
		}
	}
}
