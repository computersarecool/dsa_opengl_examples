// Transform feedback example
#include <memory>
#include <iostream>

#include "glad/glad.h"

#include "base_app.h"

static const GLchar* vertex_shader_source[] =
{
	"#version 440 core\n"

    "in float inValue;\n"

	"out float outValue;\n"

	"void main()\n"
	"{\n"
	"	outValue = sqrt(inValue);\n"
	"}\n"
};

class TransformFeedbackExample : public Application
{
private:
	void setup()
	{
		// Create vertex shader
		GLint success;
		GLchar info_log[m_log_length];
		GLuint shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shader, 1, vertex_shader_source, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, m_log_length, nullptr, info_log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << "\n" << info_log << "\n ---- " << std::endl;
		}

		// Create program
		GLuint program = glCreateProgram();
		glAttachShader(program, shader);

		// Specify transform feedback varyings
		static const GLchar* feedbackVaryings[] = { "outValue" };
		glTransformFeedbackVaryings(program, sizeof(feedbackVaryings) / sizeof(*feedbackVaryings), feedbackVaryings, GL_INTERLEAVED_ATTRIBS);

		// Link and use program
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, m_log_length, nullptr, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << "\n" << info_log << "\n ---- " << std::endl;
		}

		glUseProgram(program);

		// Input data
		GLfloat data[]{ 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };

		// Create VBO
		const GLuint flags{ 0 };
		glCreateBuffers(1, &m_vbo);
		glNamedBufferStorage(m_vbo, sizeof(data), data, flags);

		// Create and bind VAO
		GLint attrib_index{ glGetAttribLocation(program, "inValue") };
		const GLuint size{ 1 };
		const GLenum type{ GL_FLOAT };
		const GLboolean normalized{ GL_FALSE };
		const GLuint stride{ sizeof(GLfloat) };
		const GLuint offset{ 0 };
		const GLuint binding_index{ 0 };

		glCreateVertexArrays(1, &m_vao);
		glEnableVertexArrayAttrib(m_vao, static_cast<GLuint>(attrib_index));
		glVertexArrayAttribFormat(m_vao, static_cast<GLuint>(attrib_index), size, type, normalized, offset);
		glVertexArrayVertexBuffer(m_vao, binding_index, m_vbo, offset, stride);
		glVertexArrayAttribBinding(m_vao, static_cast<GLuint>(attrib_index), binding_index);
		glBindVertexArray(m_vao);

		// Create transform feedback buffer
		glCreateBuffers(1, &m_tbo);
		glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_tbo);
		glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(data), nullptr, GL_DYNAMIC_COPY);

		// Perform feedback transform
		glEnable(GL_RASTERIZER_DISCARD);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_tbo);
		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, sizeof(data) / sizeof(*data));
		glEndTransformFeedback();
		glDisable(GL_RASTERIZER_DISCARD);
		glFlush();

		// Fetch and print results
		GLfloat feedback[5];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(feedback), feedback);
		for (int i = 0; i != 5; ++i)
		{
			std::cout << feedback[i] <<std::endl;
		}
	}

	GLuint m_vao { 0 };
	GLuint m_vbo { 0 };
	GLuint m_tbo { 0 };
	const GLuint m_log_length{ 1024 };
	const GLfloat m_clear_color[4] = { 0.2f, 0.0f, 0.2f, 1.0f };
};

int main(int argc, char* argv[])
{
	std::unique_ptr<Application> app{ new TransformFeedbackExample};
	app->run();
}
