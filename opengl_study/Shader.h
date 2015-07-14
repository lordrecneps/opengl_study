#pragma once
#include <string>

class Shader
{
public:
	Shader() : program(0) {}
	Shader(const char* vert_shader, const char* frag_shader)
	{
		load_shader(vert_shader, frag_shader);
	}

	void load_shader(const char* vert_shader, const char* frag_shader)
	{
		GLuint vs_obj = read_shader(vert_shader, GL_VERTEX_SHADER);
		GLuint fs_obj = read_shader(frag_shader, GL_FRAGMENT_SHADER);

		program = glCreateProgram();
		if (program == 0)
			throw std::runtime_error("glCreateProgram failed");

		glAttachShader(program, vs_obj);
		glAttachShader(program, fs_obj);

		glLinkProgram(program);

		glDetachShader(program, vs_obj);
		glDetachShader(program, fs_obj);

		//throw exception if linking failed
		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			std::string msg("Program linking failure: ");

			GLint infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
			char* strInfoLog = new char[infoLogLength + 1];
			glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
			msg += strInfoLog;
			delete[] strInfoLog;

			glDeleteProgram(program);
			throw std::runtime_error(msg);
		}
	}

	static GLuint read_shader(const char* filename, GLenum shader_type)
	{
		std::ifstream vs(filename, std::ios::in | std::ios::binary);

		std::stringstream vs_buffer;
		vs_buffer.str("");
		vs_buffer << vs.rdbuf();

		std::string vs_str = vs_buffer.str();
		const char* vs_code = vs_str.c_str();

		GLuint vs_obj = glCreateShader(shader_type);
		if (vs_obj == 0)
			throw std::runtime_error("glCreateShader failed");

		glShaderSource(vs_obj, 1, (const GLchar**)&vs_code, NULL);
		glCompileShader(vs_obj);

		GLint status;
		glGetShaderiv(vs_obj, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			std::string msg("Compile failure in shader:\n");

			GLint infoLogLength;
			glGetShaderiv(vs_obj, GL_INFO_LOG_LENGTH, &infoLogLength);
			char* strInfoLog = new char[infoLogLength + 1];
			glGetShaderInfoLog(vs_obj, infoLogLength, NULL, strInfoLog);
			msg += strInfoLog;
			delete[] strInfoLog;

			glDeleteShader(vs_obj);
			throw std::runtime_error(msg);
		}

		return vs_obj;
	}

	GLuint getProgram() { return program; }
	void use() { glUseProgram(program); }

private:
	GLuint program;
};