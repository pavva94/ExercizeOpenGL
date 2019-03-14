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

unsigned int scr_width = 600;
unsigned int scr_height = 600;

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
GLuint carTexture;

ModelRenderer * renderer;
ModelRenderer * terra_renderer;
ModelRenderer * tavolo_renderer;
ModelRenderer * gamba_renderer;
ModelRenderer * cone_renderer;
ModelRenderer * piano_renderer;

glm::mat4 inputModelMatrix = glm::mat4(1.0);

float movimentoX = .0f;
float movimentoZ = .0f;
float movimentoXT = .0f;
float movimentoZT = .0f;

float rotazione = .0f;


void display(GLFWwindow* window)
{
	// render
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	const float PI = std::acos(-1.0f);
	glm::mat4 model_matrix = glm::mat4(1.0);
	model_matrix = inputModelMatrix * model_matrix;

	glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -30.0f, -100.0f));

	glm::mat4 projection_matrix = glm::perspective(glm::pi<float>() / 4.0f, float(scr_width) / float(scr_height), 1.0f, 205.0f);

	glm::mat4 transf = projection_matrix * view_matrix * model_matrix;
	glm::mat4 modelview = view_matrix * model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

	glm::vec3 light_position(0.f, 40.f, 75.0f);
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
	glUniform3f(colorSpecularUniformLocation, 1.f, 1.f, 1.f);
	glUniform3f(colorEmittedUniformLocation, 0.0f, 0.0f, 0.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carTexture);
	glUniform1i(colorTextureUniformLocation, 0);

	

	glm::mat4 terra_matrix = model_matrix;
	terra_matrix = glm::translate(terra_matrix, glm::vec3(0.f, -6.f, 0.f));
	terra_matrix = glm::scale(terra_matrix, glm::vec3(400.f, 0.f, 400.f));
	transf = projection_matrix * view_matrix * terra_matrix;
	modelview = view_matrix * terra_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	terra_renderer->render();


	glm::mat4 movimentoT_matrix = model_matrix;
	movimentoT_matrix = glm::translate(movimentoT_matrix, glm::vec3(movimentoXT, 0.f, movimentoZT));

	glm::mat4 gamba1_matrix = movimentoT_matrix;
	gamba1_matrix = glm::translate(gamba1_matrix, glm::vec3(-11.f, 0.f, 11.f));

	transf = projection_matrix * view_matrix * gamba1_matrix;
	modelview = view_matrix * gamba1_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	gamba_renderer->render();

	glUniform1i(hasTextureUniformLocation, 1);

	glm::mat4 movimento_matrix = movimentoT_matrix;
	movimento_matrix = glm::translate(movimento_matrix, glm::vec3(movimentoX, 0.f, movimentoZ));
	movimento_matrix = glm::rotate(movimento_matrix, rotazione, glm::vec3(.0f, 1.f, .0f));


	glm::mat4 aloe_matrix = movimento_matrix;
	aloe_matrix = glm::translate(aloe_matrix, glm::vec3(0.f, 10.5f, 0.f));
	aloe_matrix = glm::scale(aloe_matrix, glm::vec3(.2f, .2f, .2f));
	aloe_matrix = glm::rotate(aloe_matrix, -glm::pi<float>() / 2.0f, glm::vec3(1.f, 0.f, 0.f));
	transf = projection_matrix * view_matrix * aloe_matrix;
	modelview = view_matrix * aloe_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	renderer->render();

	glUniform1i(hasTextureUniformLocation, 0);

	glm::mat4 gamba2_matrix = movimentoT_matrix;
	gamba2_matrix = glm::translate(gamba2_matrix, glm::vec3(11.f, 0.f, 11.f));

	transf = projection_matrix * view_matrix * gamba2_matrix;
	modelview = view_matrix * gamba2_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	gamba_renderer->render();

	glm::mat4 gamba3_matrix = movimentoT_matrix;
	gamba3_matrix = glm::translate(gamba3_matrix, glm::vec3(-11.f, 0.f, -11.f));

	transf = projection_matrix * view_matrix * gamba3_matrix;
	modelview = view_matrix * gamba3_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	gamba_renderer->render();

	glm::mat4 gamba4_matrix = movimentoT_matrix;
	gamba4_matrix = glm::translate(gamba4_matrix, glm::vec3(11.f, 0.f, -11.f));
	
	transf = projection_matrix * view_matrix * gamba4_matrix;
	modelview = view_matrix * gamba4_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	gamba_renderer->render();

	glm::mat4 tavolo_matrix = movimentoT_matrix;
	tavolo_matrix = glm::translate(tavolo_matrix, glm::vec3(0.f, 6.f, 0.f));
	transf = projection_matrix * view_matrix * tavolo_matrix;
	modelview = view_matrix * tavolo_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	tavolo_renderer->render();

	glm::mat4 trottola_matrix = movimento_matrix;
	trottola_matrix = glm::translate(trottola_matrix, glm::vec3(0.f, 8.5f, 0.f));
	trottola_matrix = glm::rotate(trottola_matrix, glm::pi<float>() / 2.0f, glm::vec3(1.f, 0.f, 0.f));
	transf = projection_matrix * view_matrix * trottola_matrix;
	modelview = view_matrix * trottola_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	cone_renderer->render();

	glm::mat4 piano_matrix = movimento_matrix;
	piano_matrix = glm::translate(piano_matrix, glm::vec3(0.f, 10.f, 0.f));
	transf = projection_matrix * view_matrix * piano_matrix;
	modelview = view_matrix * piano_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	piano_renderer->render();

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

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		movimentoX += 2.0f * (float)time_diff;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		movimentoX -= 2.0f * (float)time_diff;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		movimentoZ += 2.0f * (float)time_diff;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		movimentoZ -= 2.0f * (float)time_diff;

	if (movimentoX > 10.f)
		movimentoX = 10.f;
	if (movimentoX < -10.f)
		movimentoX = -10.f;
	if (movimentoZ > 10.f)
		movimentoZ = 10.f;
	if (movimentoZ < -10.f)
		movimentoZ = -10.f;

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		movimentoXT += 3.0f * (float)time_diff;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		movimentoXT -= 3.0f * (float)time_diff;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		movimentoZT += 3.0f * (float)time_diff;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		movimentoZT -= 3.0f * (float)time_diff;


	rotazione += glm::pi<float>() *(float)time_diff;

}

class TerraGeometry : public IGeometry
{
public:
	TerraGeometry()
	{
	}

	~TerraGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			-1.0,   1.0,   1.0,
			1.0,   1.0,   1.0,
			1.0,   1.0,  -1.0,
			-1.0,   1.0,  -1.0,
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
		static GLfloat colors[] = {
			// front
			 0.0,   0.0, 1.0,
			 1.0,   0.0, 0.0,
			 1.0,   1.0, 1.0,
			 0.0,   1.0, 0.0,
		};

		return colors;
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
	GLsizei size() { return 6; }

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
			m_normals[vertex_counter * 3 + 2] = 0.0f;

			vertex_counter++;

			m_vertices[vertex_counter * 3 + 0] = x;
			m_vertices[vertex_counter * 3 + 1] = y;
			m_vertices[vertex_counter * 3 + 2] = zb;

			m_colors[vertex_counter * 3 + 0] = color_side.r;
			m_colors[vertex_counter * 3 + 1] = color_side.g;
			m_colors[vertex_counter * 3 + 2] = color_side.b;

			m_normals[vertex_counter * 3 + 0] = 0.0f;
			m_normals[vertex_counter * 3 + 1] = 0.0f;
			m_normals[vertex_counter * 3 + 2] = 0.0f;

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
			-12.0,  -1.0,   12.0,
			12.0,  -1.0,   12.0,
			12.0,   1.0,   12.0,
			-12.0,   1.0,   12.0,
			// back
			-12.0,  -1.0,  -12.0,
			12.0,  -1.0,  -12.0,
			12.0,   1.0,  -12.0,
			-12.0,   1.0,  -12.0,
			// left
			-12.0,  -1.0,   12.0,
			-12.0,  -1.0,  -12.0,
			-12.0,   1.0,  -12.0,
			-12.0,   1.0,   12.0,
			// right
			12.0,  -1.0,   12.0,
			12.0,  -1.0,  -12.0,
			12.0,   1.0,  -12.0,
			12.0,   1.0,   12.0,
			// bottom
			-12.0,  -1.0,   12.0,
			12.0,  -1.0,   12.0,
			12.0,  -1.0,  -12.0,
			-12.0,  -1.0,  -12.0,
			// top
			-12.0,   1.0,   12.0,
			12.0,   1.0,   12.0,
			12.0,   1.0,  -12.0,
			-12.0,   1.0,  -12.0,
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
		static GLfloat colors[] = {
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
		};

		return colors;
	}
	GLsizei verticesSize() { return 24; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2, // front
			0,  2,  3, // front

			4,  7,  6, // back
			4,  6,  5, // back

			8, 11, 10, // left
			8, 10,  9, // left

			12, 13, 14, // right
			15, 12, 14, // right

			16, 18, 17, // bottom
			16, 19, 18, // bottom

			20, 21, 22, // top
			20, 22, 23, // top
		};

		return indices;
	}
	GLsizei size() { return 36; }

	GLenum type() { return GL_TRIANGLES; }

private:
};

class GambaGeometry : public IGeometry
{
public:
	GambaGeometry()
	{
	}

	~GambaGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			// front
			-1.0,  -6.0,   1.0,
			1.0,  -6.0,   1.0,
			1.0,   6.0,   1.0,
			-1.0,   6.0,   1.0,
			// back
			-1.0,  -6.0,  -1.0,
			1.0,  -6.0,  -1.0,
			1.0,   6.0,  -1.0,
			-1.0,   6.0,  -1.0,
			// left
			-1.0,  -6.0,   1.0,
			-1.0,  -6.0,  -1.0,
			-1.0,   6.0,  -1.0,
			-1.0,   6.0,   1.0,
			// right
			1.0,  -6.0,   1.0,
			1.0,  -6.0,  -1.0,
			1.0,   6.0,  -1.0,
			1.0,   6.0,   1.0,
			// bottom
			-1.0,  -6.0,   1.0,
			1.0,  -6.0,   1.0,
			1.0,  -6.0,  -1.0,
			-1.0,  -6.0,  -1.0,
			// top
			-1.0,   6.0,   1.0,
			1.0,   6.0,   1.0,
			1.0,   6.0,  -1.0,
			-1.0,   6.0,  -1.0,
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
		static GLfloat colors[] = {
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
			0.4f, 0.2f, 0.f,
		};

		return colors;
	}
	GLsizei verticesSize() { return 24; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2, // front
			0,  2,  3, // front

			4,  7,  6, // back
			4,  6,  5, // back

			8, 11, 10, // left
			8, 10,  9, // left

			12, 13, 14, // right
			15, 12, 14, // right

			16, 18, 17, // bottom
			16, 19, 18, // bottom

			20, 21, 22, // top
			20, 22, 23, // top
		};

		return indices;
	}
	GLsizei size() { return 36; }

	GLenum type() { return GL_TRIANGLES; }

private:
};

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
			// front
			-1.5,  -0.5,   1.5,
			1.5,  -0.5,   1.5,
			1.5,   0.5,   1.5,
			-1.5,   0.5,   1.5,
			// back
			-1.5,  -0.5,  -1.5,
			1.5,  -0.5,  -1.5,
			1.5,   0.5,  -1.5,
			-1.5,   0.5,  -1.5,
			// left
			-1.5,  -0.5,   1.5,
			-1.5,  -0.5,  -1.5,
			-1.5,   0.5,  -1.5,
			-1.5,   0.5,   1.5,
			// right
			1.5,  -0.5,   1.5,
			1.5,  -0.5,  -1.5,
			1.5,   0.5,  -1.5,
			1.5,   0.5,   1.5,
			// bottom
			-1.5,  -0.5,   1.5,
			1.5,  -0.5,   1.5,
			1.5,  -0.5,  -1.5,
			-1.5,  -0.5,  -1.5,
			// top
			-1.5,   0.5,   1.5,
			1.5,   0.5,   1.5,
			1.5,   0.5,  -1.5,
			-1.5,   0.5,  -1.5,
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
		static GLfloat colors[] = {
			0.0f, 1.f, 0.f,
			0.0f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 1.f, 0.f,
		};

		return colors;
	}
	GLsizei verticesSize() { return 24; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2, // front
			0,  2,  3, // front

			4,  7,  6, // back
			4,  6,  5, // back

			8, 11, 10, // left
			8, 10,  9, // left

			12, 13, 14, // right
			15, 12, 14, // right

			16, 18, 17, // bottom
			16, 19, 18, // bottom

			20, 21, 22, // top
			20, 22, 23, // top
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
	renderer = &geo_renderer;

	TerraGeometry terra;
	ModelRenderer terraR(terra);
	terra_renderer = &terraR;

	GambaGeometry gamba;
	ModelRenderer gambaR(gamba);
	gamba_renderer = &gambaR;

	TavoloGeometry tavolo;
	ModelRenderer tavoloR(tavolo);
	tavolo_renderer = &tavoloR;

	ConeGeometry cone_geo(1.0, 3.0, glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0));
	ModelRenderer cone_geo_renderer(cone_geo);
	cone_renderer = &cone_geo_renderer;

	PianoGeometry piano;
	ModelRenderer pianoR(piano);
	piano_renderer = &pianoR;
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
	carTexture = LoadTexture("src/textures/p2_aloe.png", GL_MIRRORED_REPEAT, GL_LINEAR);

	// uncomment this call to draw in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// enable depth test
	glEnable(GL_DEPTH_TEST);

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