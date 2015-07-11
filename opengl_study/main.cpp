#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

GLFWwindow* window = 0;
GLuint program = 0;
GLuint g_VAO = 0;
GLuint g_VBO = 0;
bool swap_col = false;

static void render_scene()
{
	glClearColor(0, 0, 0, 1); // black

	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program);
	glBindVertexArray(g_VAO);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);

	glfwSwapBuffers(window);
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
		std::cout << msg << std::endl;
	}

	return vs_obj;
}

static void load_shaders()
{
	GLuint vs_obj = read_shader("vertex_shader.txt", GL_VERTEX_SHADER);
	GLuint fs_obj = read_shader("fragment_shader.txt", GL_FRAGMENT_SHADER);

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
		std::cout << msg << std::endl;
	}
}

static void load_scene() {
	// make and bind the VAO
	glGenVertexArrays(1, &g_VAO);
	glBindVertexArray(g_VAO);

	// make and bind the VBO
	glGenBuffers(1, &g_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO);

	// Put the three triangle verticies into the VBO
	GLfloat vertexData[] = {
		//  X     Y     Z
		0.0f, 0.8f, 0.0f,
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(glGetAttribLocation(program, "vert"));
	glVertexAttribPointer(glGetAttribLocation(program, "vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void on_error(int error_code, const char* msg) {
	throw std::runtime_error(msg);
}

int main()
{
	// initialise GLFW
	glfwSetErrorCallback(on_error);

	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");

	// open a window with GLFW
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(800, 600, "OpenGL Tutorial", NULL, NULL);
	if (!window)
		throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

	// GLFW settings
	glfwMakeContextCurrent(window);

	// initialise GLEW
	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("glewInit failed");

	// print out some info about the graphics drivers
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	// make sure OpenGL version 3.2 API is available
	if (!GLEW_VERSION_3_2)
		throw std::runtime_error("OpenGL 3.2 API is not available.");

	load_shaders();
	
	load_scene();
	
	while (!glfwWindowShouldClose(window)){
		// process pending events
		glfwPollEvents();

		// draw one frame
		render_scene();
	}

	// clean up and exit
	glfwTerminate();
}

