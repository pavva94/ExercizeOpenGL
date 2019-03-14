#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model_renderer.h"
#include "create_shader_program.h"
#include "compute_normals.h"
#include "load_texture.h"
#include "assimp_geometry.h"

unsigned int scr_width = 800;
unsigned int scr_height = 800;

int shaderProgram;
GLint transformationUniformLocation;
GLint modelviewUniformLocation;
// light properties
GLint lightPositionUniformLocation;
GLint lightAmbientUniformLocation;
GLint lightDiffuseUniformLocation;
GLint lightSpecularUniformLocation;
// material properties
GLint shininessUniformLocation;
GLint colorSpecularUniformLocation;
GLint colorEmittedUniformLocation;
GLint hasTextureUniformLocation;

GLint colorTextureUniformLocation;
GLuint aloeTexture;

bool wireframe = false;

float cone_rotation = 1.0f;

ModelRenderer * pianoRenderer;
ModelRenderer * piede1Rederer;
ModelRenderer * piede2Rederer;
ModelRenderer * piede3Rederer;
ModelRenderer * piede4Rederer;
ModelRenderer * tavoloRederer;
ModelRenderer * coneRenderer;
ModelRenderer * appoggiaAloeRederer;
ModelRenderer * aloeRenderer;

glm::mat4 inputModelMatrix = glm::mat4(1.0);
glm::mat4 tavoloInputMatrix = glm::mat4(1.0);

float trottola_x = 0.0;
float trottola_y = 0.0;

void display(GLFWwindow* window)
{
	// render
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// enable back face culling
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// enable depth test
		glEnable(GL_DEPTH_TEST);
	}

	const float PI = std::acos(-1.0f);
	glm::mat4 model_matrix = glm::mat4(1.0);
	model_matrix = inputModelMatrix * model_matrix;

	glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -30.0f, -90.0f));

	glm::mat4 projection_matrix = glm::perspective(glm::pi<float>() / 4.0f, float(scr_width) / float(scr_height), 1.0f, 250.0f);

	glm::mat4 transf = projection_matrix * view_matrix * model_matrix;
	glm::mat4 modelview = view_matrix * model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

	glm::vec3 light_position(-15.0f, 25.0f, 15.0f);
	glm::vec3 light_camera_position = glm::vec3(view_matrix * glm::vec4(light_position, 1.0f));
	glm::vec3 light_ambient(0.3f, 0.3f, 0.3f);
	glm::vec3 light_diffuse(0.7f, 0.7f, 0.7f);
	glm::vec3 light_specular(1.0f, 1.0f, 1.0f);
	glUniform3fv(lightPositionUniformLocation, 1, glm::value_ptr(light_camera_position));
	glUniform3fv(lightAmbientUniformLocation, 1, glm::value_ptr(light_ambient));
	glUniform3fv(lightDiffuseUniformLocation, 1, glm::value_ptr(light_diffuse));
	glUniform3fv(lightSpecularUniformLocation, 1, glm::value_ptr(light_specular));

	GLfloat shininess = 50.0f;
	glUniform1f(shininessUniformLocation, shininess);
	glUniform3f(colorSpecularUniformLocation, 0.7f, 0.7f, 0.7f);
	glUniform3f(colorEmittedUniformLocation, 0.0f, 0.0f, 0.0f);
	pianoRenderer->render();

	glm::mat4 piedi_model_matrix = tavoloInputMatrix * model_matrix;

	glm::mat4 piede1_model_matrix = piedi_model_matrix;
	piede1_model_matrix = glm::translate(piede1_model_matrix, glm::vec3(-11.0f, 0.0f, 11.0f));
	transf = projection_matrix * view_matrix * piede1_model_matrix;
	modelview = view_matrix * piede1_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	piede1Rederer->render();

	glm::mat4 piede2_model_matrix = piedi_model_matrix;
	piede2_model_matrix = glm::translate(piede2_model_matrix, glm::vec3(11.0f, 0.0f, 11.0f));
	transf = projection_matrix * view_matrix * piede2_model_matrix;
	modelview = view_matrix * piede2_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	piede2Rederer->render();

	glm::mat4 piede3_model_matrix = piedi_model_matrix;
	piede3_model_matrix = glm::translate(piede3_model_matrix, glm::vec3(-11.0f, 0.0f, -11.0f));
	transf = projection_matrix * view_matrix * piede3_model_matrix;
	modelview = view_matrix * piede3_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	piede3Rederer->render();

	glm::mat4 piede4_model_matrix = piedi_model_matrix;
	piede4_model_matrix = glm::translate(piede4_model_matrix, glm::vec3(11.0f, 0.0f, -11.0f));
	transf = projection_matrix * view_matrix * piede4_model_matrix;
	modelview = view_matrix * piede4_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	piede4Rederer->render();

	glm::mat4 tavolo_model_matrix = piedi_model_matrix;
	tavolo_model_matrix = glm::translate(tavolo_model_matrix, glm::vec3(0.0f, 7.0f, 0.0f));
	transf = projection_matrix * view_matrix * tavolo_model_matrix;
	modelview = view_matrix * tavolo_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	tavoloRederer->render();

	

	glm::mat4 trottola_model_matrix = piedi_model_matrix;
	
	trottola_model_matrix = glm::translate(trottola_model_matrix, glm::vec3(trottola_x, 0.0f, trottola_y));
	trottola_model_matrix = glm::rotate(trottola_model_matrix, cone_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 cone_model_matrix = trottola_model_matrix;
	cone_model_matrix = glm::translate(cone_model_matrix, glm::vec3(0.0f, 9.5f, 0.0f));
	cone_model_matrix = glm::rotate(cone_model_matrix, glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	transf = projection_matrix * view_matrix * cone_model_matrix;
	modelview = view_matrix * cone_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	coneRenderer->render();

	glm::mat4 appoggia_aloe_model_matrix = trottola_model_matrix;
	appoggia_aloe_model_matrix = glm::translate(appoggia_aloe_model_matrix, glm::vec3(0.0f, 11.5f, 0.0f));
	appoggia_aloe_model_matrix = glm::scale(appoggia_aloe_model_matrix, glm::vec3(0.135f, 0.5f, 0.135f));
	transf = projection_matrix * view_matrix * appoggia_aloe_model_matrix;
	modelview = view_matrix * appoggia_aloe_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	appoggiaAloeRederer->render();

	glm::mat4 aloe_model_matrix = trottola_model_matrix;
	aloe_model_matrix = glm::translate(aloe_model_matrix, glm::vec3(0.0f, 12.0f, 0.0f));
	aloe_model_matrix = glm::rotate(aloe_model_matrix, -glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	aloe_model_matrix = glm::scale(aloe_model_matrix, glm::vec3(0.3f, 0.3f, 0.3f));
	transf = projection_matrix * view_matrix * aloe_model_matrix;
	modelview = view_matrix * aloe_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aloeTexture);
	glUniform1i(colorTextureUniformLocation, 0);
	glUniform1i(hasTextureUniformLocation, 1);
	aloeRenderer->render();
	glUniform1i(hasTextureUniformLocation, 0);

	glfwSwapBuffers(window);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	scr_width = width;
	scr_height = height;

	glViewport(0, 0, width, height);
	display(window);
}

void window_refresh_callback(GLFWwindow* window)
{
	display(window);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		wireframe = !wireframe;
		display(window);
	}
}

void mouse_cursor_callback(GLFWwindow * window, double xpos, double ypos)
{
	static float prev_x = -1.0; // position at previous iteration (-1 for none)
	static float prev_y = -1.0;
	const float SPEED = 0.005f; // rad/pixel

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		if (prev_x >= 0.0f && prev_y >= 0.0f)   // if there is a previously stored position
		{
			float xdiff = float(xpos) - prev_x; // compute diff
			float ydiff = float(ypos) - prev_y;
			float delta_y = SPEED * ydiff;
			float delta_x = SPEED * xdiff;

			glm::mat4 rot = glm::mat4(1.0);    // rotate matrix
			rot = glm::rotate(rot, delta_x, glm::vec3(0.0, 1.0, 0.0));
			rot = glm::rotate(rot, delta_y, glm::vec3(1.0, 0.0, 0.0));
			inputModelMatrix = rot * inputModelMatrix;
		}

		prev_x = float(xpos); // store mouse position for next iteration
		prev_y = float(ypos);
	}
	else
	{
		prev_x = -1.0f; // mouse released: reset
		prev_y = -1.0f;
	}
}

void advance(GLFWwindow* window, double time_diff)
{
	float delta_x = 0.0;
	float delta_y = 0.0;
	const float speed = 0.5;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		delta_y = -1.0;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		delta_x = -1.0;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		delta_y = 1.0;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		delta_x = 1.0;
	delta_y *= speed * float(time_diff);
	delta_x *= speed * float(time_diff);

	glm::mat4 rot = glm::mat4(1.0);
	rot = glm::rotate(rot, delta_x, glm::vec3(0.0, 1.0, 0.0));
	rot = glm::rotate(rot, delta_y, glm::vec3(1.0, 0.0, 0.0));
	inputModelMatrix = rot * inputModelMatrix;

	cone_rotation += 2.0f * (float)time_diff;

	// traslazione tavolo
	float tavolo_x = 0.0;
	float tavolo_y = 0.0;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		tavolo_x = 3.0;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		tavolo_x = -3.0;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		tavolo_y = 3.0;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		tavolo_y = -3.0;
	tavolo_x *= float(time_diff);
	tavolo_y *= float(time_diff);

	glm::mat4 tra = glm::mat4(1.0);
	tra = glm::translate(tra, glm::vec3(tavolo_x, 0.0, 0.0));
	tra = glm::translate(tra, glm::vec3(0.0, 0.0, tavolo_y));
	tavoloInputMatrix = tra * tavoloInputMatrix;

	// traslazione trottola

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		trottola_x += -2.0f * float(time_diff);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		trottola_x += 2.0f * float(time_diff);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		trottola_y += -2.0f * float(time_diff);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		trottola_y += 2.0f * float(time_diff);

	if (trottola_x > 11.0f)
		trottola_x = 11.0f;
	if (trottola_x < -11.0f)
		trottola_x = -11.0f;
	if (trottola_y > 11.0f)
		trottola_y = 11.0f;
	if (trottola_y < -11.0f)
		trottola_y = -11.0f;
}

class PianoGeometry : public IGeometry
{
public:
	PianoGeometry()
	{
	}

	~PianoGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			// top
			-60.0,  -6.0,   -90.0,
			-60.0,  -6.0,   90.0,
			60.0,   -6.0,   90.0,
			60.0,   -6.0,   -90.0,
			
		};

		return avertices;
	}
	const GLfloat * normals()
	{
		static GLfloat anormals[] = {
			// top
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
		};

		return anormals;
	}

	const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
		};

		return acolors;
	}
	
	GLsizei verticesSize() { return 4; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2, // front
			0,  2,  3, // front
		};

		return indices;
	}
	GLsizei size() { return 36; }

	GLenum type() { return GL_TRIANGLES; }

private:

};

class PiedeGeometry : public IGeometry
{
public:
	PiedeGeometry()
	{
	}

	~PiedeGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			

			// front
			-1.0,  6.0,   1.0,
			-1.0,  -6.0,   1.0,
			1.0,   -6.0,   1.0,
			1.0,   6.0,   1.0,
			// back
			1.0,   6.0,   -1.0,
			1.0,   -6.0,   -1.0,
			-1.0,  -6.0,   -1.0,
			-1.0,  6.0,   -1.0,
			

			// left
			-1.0,  6.0,   -1.0,
			-1.0,  -6.0,  -1.0,
			-1.0,   -6.0,  1.0,
			-1.0,   6.0,   1.0,
			// right
			1.0,  6.0,   1.0,
			1.0,  -6.0,  1.0,
			1.0,   -6.0,  -1.0,
			1.0,   6.0,   -1.0,
			// top
			-1.0,   6.0,   -1.0,
			-1.0,   6.0,   1.0,
			1.0,   6.0,  1.0,
			1.0,   6.0,  -1.0,
			// bottom
			-1.0,   -6.0,   1.0,
			-1.0,   -6.0,   -1.0,
			1.0,   -6.0,  -1.0,
			1.0,   -6.0,  1.0,
		};

		return avertices;
	}
	const GLfloat * normals()
	{
		static GLfloat anormals[] = {
			// front
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			// back
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			// left
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			// right
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			// bottom
			0.0,  -1.0,   0.0,
			0.0,  -1.0,   0.0,
			0.0,  -1.0,   0.0,
			0.0,  -1.0,   0.0,
			// top
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
		};

		return anormals;
	}
	const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
		};

		return acolors;
	}
	GLsizei verticesSize() { return 24; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2, // front
			0,  2,  3, // front

			4,  5,  6, // back
			4,  6,  7, // back

			8, 9, 10, // left
			8, 10,  11, // left

			12, 13, 14, // right
			12, 14, 15, // right

			16, 17, 18, // top
			16, 18, 19, // top

			20, 21, 22, // bottom
			20, 22, 23, // bottom

		};

		return indices;
	}
	GLsizei size() { return 36; }

	GLenum type() { return GL_TRIANGLES; }

private:

};

class TavoloGeometry : public IGeometry
{
public:
	TavoloGeometry()
	{
	}

	~TavoloGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			// front
			-12.0,  1.0,   12.0,
			-12.0,  -1.0,   12.0,
			12.0,   -1.0,   12.0,
			12.0,   1.0,   12.0,
			// back
			12.0,  1.0,   -12.0,
			12.0,  -1.0,   -12.0,
			-12.0,   -1.0,   -12.0,
			-12.0,   1.0,   -12.0,
			
			// left
			-12.0,  1.0,   -12.0,
			-12.0,  -1.0,   -12.0,
			-12.0,  -1.0,   12.0,
			-12.0,  1.0,   12.0,
			// right
			12.0,   1.0,   12.0,
			12.0,   -1.0,   12.0,
			12.0,  -1.0,   -12.0,
			12.0,  1.0,   -12.0,
			
			// top
			-12.0,  1.0,   -12.0,
			-12.0,  1.0,   12.0,
			12.0,  1.0,   12.0,
			12.0,  1.0,   -12.0,
			// bottom
			-12.0,  -1.0,   12.0,
			-12.0,  -1.0,   -12.0,
			12.0,  -1.0,   -12.0,
			12.0,  -1.0,   12.0,
		};

		return avertices;
	}
	const GLfloat * normals()
	{
		static GLfloat anormals[] = {
			// front
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			// back
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			// left
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			// right
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			// top
			0.0,  1.0,   0.0,
			0.0,  1.0,   0.0,
			0.0,  1.0,   0.0,
			0.0,  1.0,   0.0,
			// bottom
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
		};

		return anormals;
	}
	const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
			0.4,   0.2,   0.0,
		};

		return acolors;
	}
	GLsizei verticesSize() { return 24; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2, // front
			0,  2,  3, // front

			4,  5,  6, // back
			4,  6,  7, // back

			8, 9, 10, // left
			8, 10,  11, // left

			12, 13, 14, // right
			12, 14, 15, // right

			16, 17, 18, // top
			16, 18, 19, // top

			20, 21, 22, // bottom
			20, 22, 23, // bottom

		};

		return indices;
	}
	GLsizei size() { return 36; }

	GLenum type() { return GL_TRIANGLES; }

private:

};

class ConeGeometry : public IGeometry
{
public:
	ConeGeometry(float radius, float height, glm::vec3 color_base, glm::vec3 color_side)
	{
		const int SAMPLES = 20;
		m_vertices = new GLfloat[3 * (3 * SAMPLES)];
		m_colors = new GLfloat[3 * (3 * SAMPLES)];
		m_normals = new GLfloat[3 * (3 * SAMPLES)];
		m_faces = new GLuint[3 * (2 * SAMPLES - 2)];

		int face_counter = 0;
		int vertex_counter = 0;

		int zero_vertex;
		// base
		zero_vertex = vertex_counter;
		for (int i = 0; i < SAMPLES; i++)
		{
			const float angle = float(i) / float(SAMPLES) * glm::pi<float>() * 2.0f;
			const float x = radius * std::cos(angle);
			const float y = radius * std::sin(angle);
			const float z = -height / 2.0f;

			m_vertices[vertex_counter * 3 + 0] = x;
			m_vertices[vertex_counter * 3 + 1] = y;
			m_vertices[vertex_counter * 3 + 2] = z;

			m_colors[vertex_counter * 3 + 0] = color_base.r;
			m_colors[vertex_counter * 3 + 1] = color_base.g;
			m_colors[vertex_counter * 3 + 2] = color_base.b;

			m_normals[vertex_counter * 3 + 0] = 0.0f;
			m_normals[vertex_counter * 3 + 1] = 0.0f;
			m_normals[vertex_counter * 3 + 2] = -1.0f;
			vertex_counter++;

			if (i >= 2)
			{
				m_faces[face_counter * 3 + 0] = zero_vertex;
				m_faces[face_counter * 3 + 1] = vertex_counter - 1;
				m_faces[face_counter * 3 + 2] = vertex_counter - 2;
				face_counter++;
			}
		}

		// apex
		glm::vec3 apex(0.0f, 0.0f, height / 2.0f);

		// lateral surface: vertices
		zero_vertex = vertex_counter;
		int zero_face;
		zero_face = face_counter;
		for (int i = 0; i < SAMPLES; i++)
		{
			const float angle = float(i) / float(SAMPLES) * glm::pi<float>() * 2.0f;
			const float x = radius * std::cos(angle);
			const float y = radius * std::sin(angle);
			const float zb = -height / 2.0f;

			m_vertices[vertex_counter * 3 + 0] = apex.x;
			m_vertices[vertex_counter * 3 + 1] = apex.y;
			m_vertices[vertex_counter * 3 + 2] = apex.z;

			m_colors[vertex_counter * 3 + 0] = color_side.r;
			m_colors[vertex_counter * 3 + 1] = color_side.g;
			m_colors[vertex_counter * 3 + 2] = color_side.b;

			// initialize to 0 for now
			m_normals[vertex_counter * 3 + 0] = 0.0f;
			m_normals[vertex_counter * 3 + 1] = 0.0f;
			m_normals[vertex_counter * 3 + 2] = 1.0f;

			vertex_counter++;

			m_vertices[vertex_counter * 3 + 0] = x;
			m_vertices[vertex_counter * 3 + 1] = y;
			m_vertices[vertex_counter * 3 + 2] = zb;

			m_colors[vertex_counter * 3 + 0] = color_side.r;
			m_colors[vertex_counter * 3 + 1] = color_side.g;
			m_colors[vertex_counter * 3 + 2] = color_side.b;

			m_normals[vertex_counter * 3 + 0] = 0.0f;
			m_normals[vertex_counter * 3 + 1] = 0.0f;
			m_normals[vertex_counter * 3 + 2] = 1.0f;

			vertex_counter++;
		}

		// lateral surface: faces
		for (int i = 0; i < SAMPLES; i++)
		{
			int i0 = zero_vertex + i * 2;  // apex
			int i1 = zero_vertex + i * 2 + 1;
			int i2 = zero_vertex + ((i + 1) % SAMPLES) * 2 + 1;

			m_faces[face_counter * 3 + 0] = i0;
			m_faces[face_counter * 3 + 1] = i1;
			m_faces[face_counter * 3 + 2] = i2;

			face_counter++;

			glm::vec3 v0(m_vertices[i0 * 3 + 0], m_vertices[i0 * 3 + 1], m_vertices[i0 * 3 + 2]);
			glm::vec3 v1(m_vertices[i1 * 3 + 0], m_vertices[i1 * 3 + 1], m_vertices[i1 * 3 + 2]);
			glm::vec3 v2(m_vertices[i2 * 3 + 0], m_vertices[i2 * 3 + 1], m_vertices[i2 * 3 + 2]);
			glm::vec3 normal = computeNormal(v0, v1, v2);

			m_normals[i0 * 3 + 0] += normal.x;
			m_normals[i0 * 3 + 1] += normal.y;
			m_normals[i0 * 3 + 2] += normal.z;

			m_normals[i1 * 3 + 0] += normal.x;
			m_normals[i1 * 3 + 1] += normal.y;
			m_normals[i1 * 3 + 2] += normal.z;

			m_normals[i2 * 3 + 0] += normal.x;
			m_normals[i2 * 3 + 1] += normal.y;
			m_normals[i2 * 3 + 2] += normal.z;
		}

		// normalize normals
		for (int i = zero_vertex; i < vertex_counter; i++)
		{
			glm::vec3 normal(m_normals[i * 3 + 0], m_normals[i * 3 + 1], m_normals[i * 3 + 2]);
			normal = glm::normalize(normal);
			m_normals[i * 3 + 0] = normal.x;
			m_normals[i * 3 + 1] = normal.y;
			m_normals[i * 3 + 2] = normal.z;
		}

		m_faces_size = face_counter;
		m_vertices_size = vertex_counter;
	}

	~ConeGeometry()
	{
		delete[] m_vertices;
		delete[] m_colors;
		delete[] m_normals;
		delete[] m_faces;
	}

	const GLfloat * vertices() { return m_vertices; }
	const GLfloat * colors() { return m_colors; }
	const GLfloat * normals() { return m_normals; }
	const GLuint * faces() { return m_faces; }
	GLsizei verticesSize() { return m_vertices_size; }
	GLsizei size() { return m_faces_size * 3; }

	GLenum type() { return GL_TRIANGLES; }

private:
	GLfloat * m_vertices;
	GLfloat * m_colors;
	GLfloat * m_normals;
	GLsizei m_vertices_size;

	GLuint * m_faces;
	GLsizei m_faces_size;
};

class AppoggiaAloeGeometry : public IGeometry
{
public:
	AppoggiaAloeGeometry()
	{
	}

	~AppoggiaAloeGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			// front
			-12.0,  1.0,   12.0,
			-12.0,  -1.0,   12.0,
			12.0,   -1.0,   12.0,
			12.0,   1.0,   12.0,
			// back
			12.0,  1.0,   -12.0,
			12.0,  -1.0,   -12.0,
			-12.0,   -1.0,   -12.0,
			-12.0,   1.0,   -12.0,

			// left
			-12.0,  1.0,   -12.0,
			-12.0,  -1.0,   -12.0,
			-12.0,  -1.0,   12.0,
			-12.0,  1.0,   12.0,
			// right
			12.0,   1.0,   12.0,
			12.0,   -1.0,   12.0,
			12.0,  -1.0,   -12.0,
			12.0,  1.0,   -12.0,

			// top
			-12.0,  1.0,   -12.0,
			-12.0,  1.0,   12.0,
			12.0,  1.0,   12.0,
			12.0,  1.0,   -12.0,
			// bottom
			-12.0,  -1.0,   12.0,
			-12.0,  -1.0,   -12.0,
			12.0,  -1.0,   -12.0,
			12.0,  -1.0,   12.0,
		};

		return avertices;
	}
	const GLfloat * normals()
	{
		static GLfloat anormals[] = {
			// front
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			0.0,   0.0,   1.0,
			// back
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			0.0,   0.0,  -1.0,
			// left
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			-1.0,   0.0,   0.0,
			// right
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			1.0,   0.0,   0.0,
			// top
			0.0,  1.0,   0.0,
			0.0,  1.0,   0.0,
			0.0,  1.0,   0.0,
			0.0,  1.0,   0.0,
			// bottom
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
		};

		return anormals;
	}
	const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
			0.0,   1.0,   0.0,
		};

		return acolors;
	}
	GLsizei verticesSize() { return 24; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2, // front
			0,  2,  3, // front

			4,  5,  6, // back
			4,  6,  7, // back

			8, 9, 10, // left
			8, 10,  11, // left

			12, 13, 14, // right
			12, 14, 15, // right

			16, 17, 18, // top
			16, 18, 19, // top

			20, 21, 22, // bottom
			20, 22, 23, // bottom

		};

		return indices;
	}
	GLsizei size() { return 36; }

	GLenum type() { return GL_TRIANGLES; }

private:

};

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowRefreshCallback(window, window_refresh_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_cursor_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	AssimpGeometry geo("src/p2_aloe.obj");
	ModelRenderer geo_renderer(geo);
	aloeRenderer = &geo_renderer;
	
	PianoGeometry piano;
	ModelRenderer piano_renderer(piano);
	pianoRenderer = &piano_renderer;

	PiedeGeometry piede1;
	ModelRenderer piede1_renderer(piede1);
	piede1Rederer = &piede1_renderer;

	PiedeGeometry piede2;
	ModelRenderer piede2_renderer(piede2);
	piede2Rederer = &piede2_renderer;

	PiedeGeometry piede3;
	ModelRenderer piede3_renderer(piede3);
	piede3Rederer = &piede3_renderer;

	PiedeGeometry piede4;
	ModelRenderer piede4_renderer(piede4);
	piede4Rederer = &piede4_renderer;

	TavoloGeometry tavolo;
	ModelRenderer tavolo_renderer(tavolo);
	tavoloRederer = &tavolo_renderer;

	ConeGeometry cone_geo(1.0, 3.0, glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0));
	ModelRenderer cone_geo_renderer(cone_geo);
	coneRenderer = &cone_geo_renderer;

	AppoggiaAloeGeometry appoggia_aloe;
	ModelRenderer appoggia_aloe_renderer(appoggia_aloe);
	appoggiaAloeRederer = &appoggia_aloe_renderer;
	

	// load GLSL shaders
	shaderProgram = createShaderProgram("es10_texture_and_vcolor.vert", "es10_texture_and_vcolor.frag");
	transformationUniformLocation = glGetUniformLocation(shaderProgram, "transformation");
	modelviewUniformLocation = glGetUniformLocation(shaderProgram, "modelview");

	lightPositionUniformLocation = glGetUniformLocation(shaderProgram, "light_position");
	lightAmbientUniformLocation = glGetUniformLocation(shaderProgram, "light_ambient");
	lightDiffuseUniformLocation = glGetUniformLocation(shaderProgram, "light_diffuse");
	lightSpecularUniformLocation = glGetUniformLocation(shaderProgram, "light_specular");

	shininessUniformLocation = glGetUniformLocation(shaderProgram, "shininess");
	colorSpecularUniformLocation = glGetUniformLocation(shaderProgram, "color_specular");
	colorEmittedUniformLocation = glGetUniformLocation(shaderProgram, "color_emitted");

	hasTextureUniformLocation = glGetUniformLocation(shaderProgram, "has_texture");

	colorTextureUniformLocation = glGetUniformLocation(shaderProgram, "color_texture");
	aloeTexture = LoadTexture("src/textures/p2_aloe.png", GL_MIRRORED_REPEAT, GL_LINEAR);

	



	// render loop
	// -----------
	double curr_time = glfwGetTime();
	double prev_time;
	while (!glfwWindowShouldClose(window))
	{
		display(window);
		glfwWaitEventsTimeout(0.01);

		prev_time = curr_time;
		curr_time = glfwGetTime();
		advance(window, curr_time - prev_time);
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}