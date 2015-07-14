#pragma once
#include "Model.h"
#include "PointLight.h"
#include <glm/glm.hpp>

class Instance
{
public:
	Instance() : m(0), transform() {}

	void render(const glm::mat4& camera, const glm::vec3& eye, const PointLight& light)
	{
		m->shader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m->tex);
		glUniform1i(glGetUniformLocation(m->shader.getProgram(), "tex"), 0);
		glUniform1f(glGetUniformLocation(m->shader.getProgram(), "ambient"), light.ambient);
		glUniform1f(glGetUniformLocation(m->shader.getProgram(), "shininess"), m->shininess);
		glUniform1f(glGetUniformLocation(m->shader.getProgram(), "attenuation"), light.attenuation);

		glUniformMatrix4fv(glGetUniformLocation(m->shader.getProgram(), "camera"), 1, GL_FALSE, glm::value_ptr(camera));

		glUniformMatrix4fv(glGetUniformLocation(m->shader.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(transform));
		glUniformMatrix3fv(glGetUniformLocation(m->shader.getProgram(), "normal_matrix"), 1, GL_FALSE, glm::value_ptr(invTranspose));

		glUniform3fv(glGetUniformLocation(m->shader.getProgram(), "lightPos"), 1, glm::value_ptr(light.pos));
		glUniform3fv(glGetUniformLocation(m->shader.getProgram(), "lightCol"), 1, glm::value_ptr(light.color));
		glUniform3fv(glGetUniformLocation(m->shader.getProgram(), "cameraPos"), 1, glm::value_ptr(eye));
		glUniform3fv(glGetUniformLocation(m->shader.getProgram(), "specColor"), 1, glm::value_ptr(m->specular_color));

		glBindVertexArray(m->VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3 * 2 * 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}

	void setTransform(const glm::mat4 T)
	{
		transform = T;
		invTranspose = glm::transpose(glm::inverse(glm::mat3(transform)));
	}

	Model* m;
	glm::mat4 transform;
	glm::mat3 invTranspose;
};