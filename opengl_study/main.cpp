#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


GLFWwindow* window = 0;
GLuint program = 0;
GLuint g_VAO = 0;
GLuint g_VBO = 0;
GLuint tex = 0;
cv::Mat img;
bool swap_col = false;

glm::vec3 cam_pos;
float cam_vAngle = 0.0f, cam_hAngle = 0.0f;
double cam_zoom = 0.0;
float cam_fov = 50.0f;

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
		throw std::runtime_error(msg);
	}
}

static void load_scene()
{
	// make and bind the VAO
	glGenVertexArrays(1, &g_VAO);
	glBindVertexArray(g_VAO);

	// make and bind the VBO
	glGenBuffers(1, &g_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_VBO);

	// Put the three triangle verticies into the VBO
	GLfloat vertexData[] = {
		//  X     Y     Z       U     V
		// bottom
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,

		// top
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		// front
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		// back
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f,

		// left
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f,

		// right
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(glGetAttribLocation(program, "vert"));
	glVertexAttribPointer(glGetAttribLocation(program, "vert"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);

	glEnableVertexAttribArray(glGetAttribLocation(program, "vertTexCoord"));
	glVertexAttribPointer(glGetAttribLocation(program, "vertTexCoord"), 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	// unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static void load_textures()
{
	cv::Mat src_img = cv::imread("wooden-crate.jpg", CV_LOAD_IMAGE_COLOR);
	cv::flip(src_img, img, 0);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		(GLsizei)img.cols,
		(GLsizei)img.rows,
		0,
		GL_BGR,
		GL_UNSIGNED_BYTE,
		img.data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

static void render_scene()
{
	glClearColor(0, 0, 0, 1); // black

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	glm::mat4 camera = glm::perspective(glm::radians(cam_fov), 800 / 600.0f, 0.1f, 10.0f);
	
	glm::mat4 orientation;
	orientation = glm::rotate(orientation, glm::radians(cam_vAngle), glm::vec3(1, 0, 0));
	orientation = glm::rotate(orientation, glm::radians(cam_hAngle), glm::vec3(0, 1, 0));
	camera *= orientation;

	camera = glm::translate(camera, -cam_pos);

	//glm::mat4 camera = glm::lookAt(cam_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(program, "camera"), 1, GL_FALSE, glm::value_ptr(camera));

	float box_rot_angle = (float) glfwGetTime() * 45;
	glm::mat4 model = glm::rotate(glm::mat4(), glm::radians(box_rot_angle), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glm::vec3 forward = glm::vec3(glm::inverse(orientation) * glm::vec4(0, 0, -1, 1));
	glm::vec3 up = glm::vec3(glm::inverse(orientation) * glm::vec4(0, 1, 0, 1));
	glm::vec3 right = glm::vec3(glm::inverse(orientation) * glm::vec4(1, 0, 0, 1));

	if (glfwGetKey(window, 'S')){
		cam_pos -= 0.1f * forward;
	}
	else if (glfwGetKey(window, 'W')){
		cam_pos += 0.1f * forward;
	}
	if (glfwGetKey(window, 'A')){
		cam_pos -= 0.1f * right;
	}
	else if (glfwGetKey(window, 'D')){
		cam_pos += 0.1f * right;
	}
	if (glfwGetKey(window, 'X')){
		cam_pos -= 0.1f * up;
	}
	else if (glfwGetKey(window, 'Z')){
		cam_pos += 0.1f * up;
	}

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	cam_vAngle += 0.1f * (float)mouseY;
	cam_hAngle += 0.1f * (float)mouseX;

	cam_hAngle = fmodf(cam_hAngle, 360.0f);
	cam_vAngle = (cam_vAngle > 85.0f) ? 85.0f: cam_vAngle;
	cam_vAngle = (cam_vAngle < -85.0f) ? -85.0f : cam_vAngle;
	glfwSetCursorPos(window, 0, 0);

	const float zoomSensitivity = -1.5f;
	float fieldOfView = cam_fov + zoomSensitivity * (float)cam_zoom;
	if (fieldOfView < 5.0f) fieldOfView = 5.0f;
	if (fieldOfView > 130.0f) fieldOfView = 130.0f;
	
	cam_fov = fieldOfView;
	cam_zoom = 0;

	glBindVertexArray(g_VAO);

	glDrawArrays(GL_TRIANGLES, 0, 3*2*6);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	glfwSwapBuffers(window);
}

void on_error(int error_code, const char* msg) {
	throw std::runtime_error(msg);
}

void OnScroll(GLFWwindow* window, double deltaX, double deltaY) {
	cam_zoom += deltaY;
}

void dain()
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, 0, 0);
	glfwSetScrollCallback(window, OnScroll);

	// initialise GLEW
	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("glewInit failed");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// print out some info about the graphics drivers
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	// make sure OpenGL version 3.2 API is available
	if (!GLEW_VERSION_3_2)
		throw std::runtime_error("OpenGL 3.2 API is not available.");

	load_shaders();

	load_textures();
	
	load_scene();
	
	while (!glfwWindowShouldClose(window)){
		// process pending events
		glfwPollEvents();

		// draw one frame
		render_scene();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// clean up and exit
	glfwTerminate();
}


int main(int argc, char *argv[]) {
	try {
		dain();
	}
	catch (const std::exception& e){
		std::cerr << "ERROR: " << e.what() << std::endl;
		int bla;
		std::cin >> bla;
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}