#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <GL/glew.h>

#include "Shader.h"

class Model
{
public:
	Model() : VAO(0), VBO(0), tex(0), shininess(0.0f), specular_color(1, 1, 1) {}

	void load_textures(const char* texture_file)
	{
		cv::Mat src_img = cv::imread(texture_file, CV_LOAD_IMAGE_COLOR);
		cv::flip(src_img, img, 0);

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_SRGB,
			(GLsizei)img.cols,
			(GLsizei)img.rows,
			0,
			GL_BGR,
			GL_UNSIGNED_BYTE,
			img.data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint VAO;
	GLuint VBO;
	GLuint tex;
	cv::Mat img;
	Shader shader;

	float shininess;
	glm::vec3 specular_color;

	GLenum drawType;
	GLint drawStart;
	GLint drawCount;
};