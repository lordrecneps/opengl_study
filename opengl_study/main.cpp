#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Shader.h"
#include "Model.h"
#include "Instance.h"
#include "Camera.h"

GLFWwindow* window = 0;

Model box;
std::vector<Instance> instances;
Camera cam(50.0f, -1.5f);
PointLight light(glm::vec3(), glm::vec3(1, 1, 1), 0.005f, 0.05f);


static void load_box()
{
	box.shader.load_shader("vertex_shader.txt", "fragment_shader.txt");
	box.load_textures("wooden-crate.jpg");
	box.shininess = 720;

	// make and bind the VAO
	glGenVertexArrays(1, &box.VAO);
	glBindVertexArray(box.VAO);

	// make and bind the VBO
	glGenBuffers(1, &box.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, box.VBO);

	// Put the three triangle verticies into the VBO
	GLfloat vertexData[] = {
		//  X     Y     Z       U     V          Normal
		// bottom
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		// top
		-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		// front
		-1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		// back
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,

		// left
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		// right
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(glGetAttribLocation(box.shader.getProgram(), "vert"));
	glVertexAttribPointer(glGetAttribLocation(box.shader.getProgram(), "vert"), 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), NULL);

	glEnableVertexAttribArray(glGetAttribLocation(box.shader.getProgram(), "vertTexCoord"));
	glVertexAttribPointer(glGetAttribLocation(box.shader.getProgram(), "vertTexCoord"), 2, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(glGetAttribLocation(box.shader.getProgram(), "vertNormal"));
	glVertexAttribPointer(glGetAttribLocation(box.shader.getProgram(), "vertNormal"), 3, GL_FLOAT, GL_TRUE, 8 * sizeof(GLfloat), (const GLvoid*)(5 * sizeof(GLfloat)));

	// unbind the VBO and VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static void load_scene()
{
	Instance dot;
	dot.m = &box;
	dot.setTransform(glm::mat4());
	instances.push_back(dot);

	Instance i;
	i.m = &box;
	i.setTransform(glm::translate(glm::mat4(), glm::vec3(0, -4, 0)) * glm::scale(glm::mat4(), glm::vec3(1, 2, 1)));
	instances.push_back(i);

	Instance hLeft;
	hLeft.m = &box;
	hLeft.setTransform(glm::translate(glm::mat4(), glm::vec3(-8, 0, 0)) * glm::scale(glm::mat4(), glm::vec3(1, 6, 1)));
	instances.push_back(hLeft);

	Instance hRight;
	hRight.m = &box;
	hRight.setTransform(glm::translate(glm::mat4(), glm::vec3(-4, 0, 0)) * glm::scale(glm::mat4(), glm::vec3(1, 6, 1)));
	instances.push_back(hRight);

	Instance hMid;
	hMid.m = &box;
	hMid.setTransform(glm::translate(glm::mat4(), glm::vec3(-6, 0, 0)) * glm::scale(glm::mat4(), glm::vec3(2, 1, 0.8)));
	instances.push_back(hMid);
}

static void render_scene()
{
	glClearColor(0, 0, 0, 1); // black

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 forward, up, right;
	glm::mat4 camera = cam.get_matrix(&forward, &up, &right);

	float box_rot_angle = (float)glfwGetTime() * 45;
	instances[0].setTransform(glm::rotate(glm::mat4(), glm::radians(box_rot_angle), glm::vec3(0, 1, 0)));

	light.pos = cam.pos;

	for (auto itr : instances)
	{
		itr.render(camera, cam.pos, light);
	}

	if (glfwGetKey(window, 'S')){
		cam.pos -= 0.1f * forward;
	}
	else if (glfwGetKey(window, 'W')){
		cam.pos += 0.1f * forward;
	}
	if (glfwGetKey(window, 'A')){
		cam.pos -= 0.1f * right;
	}
	else if (glfwGetKey(window, 'D')){
		cam.pos += 0.1f * right;
	}
	if (glfwGetKey(window, 'X')){
		cam.pos -= 0.1f * up;
	}
	else if (glfwGetKey(window, 'Z')){
		cam.pos += 0.1f * up;
	}

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	cam.update_angles(0.1f * (float)mouseY, 0.1f * (float)mouseX);
	glfwSetCursorPos(window, 0, 0);

	glfwSwapBuffers(window);
}

void on_error(int error_code, const char* msg) {
	throw std::runtime_error(msg);
}

void OnScroll(GLFWwindow* window, double deltaX, double deltaY) {
	cam.adjust_zoom((float) deltaY);
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

	load_box();	
	load_scene();
	
	while (!glfwWindowShouldClose(window)){
		// process pending events
		glfwPollEvents();

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