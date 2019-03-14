#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include <stb_image.h>

#include "compute_normals.h"

#include "model_renderer.h"
#include "create_shader_program.h"
#include "load_texture.h"

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
// texture
GLint colorTextureUniformLocation;
GLuint colorTexture;
GLint hasTextureUniformLocation;

ModelRenderer * renderer;
ModelRenderer * cone_renderer;
ModelRenderer * sphere_renderer;

bool cambioObject = true;
int numeroScalini = 10;
float movimentoX, movimentoY = 0.0f;

glm::mat4 inputModelMatrix = glm::mat4(1.0); // initialize to identity

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

    glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -370.0f));

    glm::mat4 projection_matrix = glm::perspective(glm::pi<float>() / 4.0f, float(scr_width) / float(scr_height), 0.1f, 705.0f);

    glm::mat4 transf = projection_matrix * view_matrix * model_matrix;
    glm::mat4 modelview = view_matrix * model_matrix;
    glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
    glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

    glm::vec3 light_position(0.0f, -50.0f, 500.0f);
    glm::vec3 light_camera_position = glm::vec3(view_matrix * glm::vec4(light_position, 1.0f));
    glm::vec3 light_ambient(0.3f, 0.3f, 0.3f);
    glm::vec3 light_diffuse(0.7f, 0.7f, 0.7f);
    glm::vec3 light_specular(1.0f, 1.0f, 1.0f);
    glUniform3fv(lightPositionUniformLocation, 1, glm::value_ptr(light_camera_position));
    glUniform3fv(lightAmbientUniformLocation, 1, glm::value_ptr(light_ambient));
    glUniform3fv(lightDiffuseUniformLocation, 1, glm::value_ptr(light_diffuse));
    glUniform3fv(lightSpecularUniformLocation, 1, glm::value_ptr(light_specular));

    GLfloat shininess = 100.0f;
    glUniform1f(shininessUniformLocation, shininess);
    glUniform3f(colorSpecularUniformLocation, 0.1f, 0.1f, 0.1f);
    glUniform3f(colorEmittedUniformLocation, 0.0f, 0.0f, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glUniform1i(colorTextureUniformLocation, 0);

	glm::mat4 prismModelMatrix = model_matrix;
	prismModelMatrix = glm::translate(prismModelMatrix, glm::vec3(-100.0f, 70.0f, 0.0f));
	for (int i = 0; i < numeroScalini; i++) {
		glUniform1i(hasTextureUniformLocation, 1);
		prismModelMatrix = glm::translate(prismModelMatrix, glm::vec3(20.0f, -20.0f, 0.0f));
		glm::mat4 transf = projection_matrix * view_matrix * prismModelMatrix;
		glm::mat4 modelview = view_matrix * prismModelMatrix;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		renderer->render();
		glUniform1i(hasTextureUniformLocation, 0);
	}

	for (int i = 0; i < 5; i++) {

		int gradino = movimentoX / 20.0f;
		
		float spostamentoRelativo = movimentoX - gradino * 20.0f;
		movimentoY = -0.2 * spostamentoRelativo * spostamentoRelativo + 3.0f * spostamentoRelativo;
		movimentoY += -20.0 * gradino;
		spostamentoRelativo += 20.0 * gradino;

		glm::mat4 cone_model_matrix = model_matrix;
		cone_model_matrix = glm::translate(cone_model_matrix, glm::vec3(glm::vec3(spostamentoRelativo - 80.0, movimentoY + 55.0, i * 4.0 - 8.0)));
		transf = projection_matrix * view_matrix * cone_model_matrix;
		modelview = view_matrix * cone_model_matrix;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		if (cambioObject)
			cone_renderer->render();
		else
			sphere_renderer->render();
	}/*

	for (int i = 0; i < 5; i++)
	{
		// numero del gradino, parte intera indica il gradino superato parte decimale di quanto mi sono spostato
		// ma il cast a int prende solo la parte intera 
		int i_gradino = movimentoX / 20.0;
		// posizione X del gradino
		//float f_gradino = ;
		// quanto mi sono spostato dalla x del gradino precedente
		float x = movimentoX - i_gradino * 20.0;
		// calcolo y secondo la parabola
		float y = -0.2 * x * x + 3.0 * x;
		// dao che il gradino va in basso devo togliere 20
		y += -20.0 * i_gradino;
		//  è uguale da object_x
		x += 20.0 * i_gradino;
		// sollevo un pochino la palla dal gradino
		y += 2.0;

		glm::mat4 step_model_matrix = model_matrix;
		step_model_matrix = glm::translate(step_model_matrix, glm::vec3(x - 80.0, y + 55.0, i * 4.0 - 8.0));
		step_model_matrix = glm::rotate(step_model_matrix, glm::pi<float>() / 2.0f, glm::vec3(1, 0, 0));
		glm::mat4 transf = projection_matrix * view_matrix * step_model_matrix;
		glm::mat4 modelview = view_matrix * step_model_matrix;

		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

		if (cambioObject)
			cone_renderer->render();
		else
			sphere_renderer->render();
	}
	*/
	

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

	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		if (numeroScalini > 2)
			numeroScalini -= 1;
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		if (numeroScalini < 10)
			numeroScalini += 1;
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		cambioObject = !cambioObject;
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

	movimentoX += 25.0 * (float)time_diff;
	if (movimentoX > 20*numeroScalini)
		movimentoX = 0.0f;
}

class PrismGeometry : public IGeometry
{
public:
	PrismGeometry()
    {
    }

    ~PrismGeometry()
    {
    }

    const GLfloat * vertices()
    {
        static GLfloat avertices[] = {
			//front
            -5.0f, 4.0f, 15.0f,
            0.0f, -4.0f, 15.0f,
            5.0f, 4.0f, 15.0f,
			//back
			5.0f, 4.0f, -15.0f,
			0.0f, -4.0f, -15.0f,//4
			-5.0f, 4.0f, -15.0f,//5
			// right
			5.0f, 4.0f, 15.0f,
			0.0f, -4.0f, 15.0f,
			0.0f, -4.0f, -15.0f,
			5.0f, 4.0f, -15.0f,//9
			// left
			-5.0f, 4.0f, -15.0f,//10
			0.0f, -4.0f, -15.0f,
			0.0f, -4.0f, 15.0f,
			-5.0f, 4.0f, 15.0f,//13
			//top
			-5.0f, 4.0f, 15.0f,
			5.0f, 4.0f, 15.0f,
			5.0f, 4.0f, -15.0f,//16
			-5.0f, 4.0f, -15.0f,//17
        };

        return avertices;
    }
    const GLfloat * normals()
    {
        static GLfloat anormals[] = {
			//front
             0.0,   0.0,   1.0,
             0.0,   0.0,   1.0,
             0.0,   0.0,   1.0,
			 //back
			 0.0,   0.0,   -1.0,
			 0.0,   0.0,   -1.0,
			 0.0,   0.0,   -1.0,
			 //right
			 normalRight.x, normalRight.y, normalRight.z,
			 normalRight.x, normalRight.y, normalRight.z,
			 normalRight.x, normalRight.y, normalRight.z,
			 normalRight.x, normalRight.y, normalRight.z,
			 //left
			 normalLeft.x, normalLeft.y, normalLeft.z,
			 normalLeft.x, normalLeft.y, normalLeft.z,
			 normalLeft.x, normalLeft.y, normalLeft.z,
			 normalLeft.x, normalLeft.y, normalLeft.z,
			 //top
			 0.0,   1.0,   0.0,
			 0.0,   1.0,   0.0,
			 0.0,   1.0,   0.0,
			 0.0,   1.0,   0.0,
        };

        return anormals;
    }
    const GLfloat * texCoords()
    {
        static GLfloat atexcoords[] = {
			//front
            0.5f, 1.0f,
            0.25f, 0.6f,
            0.0f, 1.0f,
			//back
			1.0f, 1.0f,
			0.75f, 0.6f,
			0.5f, 1.0f,
			//right
			0.0f, 0.0f,
			0.0f, 0.18f,
			1.0f, 0.18f,
			1.0f, 0.0f,
			//left
			0.0f, 0.18f,
			0.0f, 0.34f,
			1.0f, 0.18f,
			1.0f, 0.34f,
			//top
			0.0f, 0.34f,
			0.0f, 0.5f,
			1.0f, 0.5f,
			1.0f, 0.34f,
        };

        return atexcoords;
    }
    GLsizei verticesSize() { return 18; }
    const GLuint * faces()
    {
        static unsigned int indices[] = {
            0,  1,  2,
			3, 4, 5, 
			6, 7, 8, 
			6, 8, 9, 
			10, 11, 13, 
			11, 12, 13, 
			14, 15, 17, 
			15, 16, 17,
        };

        return indices;
    }
    GLsizei size() { return 24; }

    GLenum type() { return GL_TRIANGLES; }

private:
	glm::vec3 normalRight = computeNormal(glm::vec3(5.0f, 4.0f, 15.0f), glm::vec3(0.0f, -4.0f, 15.0f), glm::vec3(0.0f, -4.0f, -15.0f));
	glm::vec3 normalLeft = computeNormal(glm::vec3(-5.0f, 4.0f, -15.0f), glm::vec3(0.0f, -4.0f, -15.0f), glm::vec3(0.0f, -4.0f, 15.0f));
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

class SphereGeometry : public IGeometry
{
public:
	SphereGeometry(float radius, glm::vec3 color)
	{
		const int SAMPLES_LAT = 16;
		const int SAMPLES_LON = 16;
		m_vertices = new GLfloat[3 * ((SAMPLES_LAT - 2) * SAMPLES_LON + 2)];
		m_colors = new GLfloat[3 * ((SAMPLES_LAT - 2) * SAMPLES_LON + 2)];
		m_normals = new GLfloat[3 * ((SAMPLES_LAT - 2) * SAMPLES_LON + 2)];
		m_faces = new GLuint[3 * (2 * (SAMPLES_LAT - 3) + 2) * SAMPLES_LON];

		int face_counter = 0;
		int vertex_counter = 0;

		// generate vertices
		for (int vi = 1; vi < SAMPLES_LAT - 1; vi++)
			for (int hi = 0; hi < SAMPLES_LON; hi++)
			{
				float angle_lon = float(hi) / float(SAMPLES_LON) * glm::pi<float>() * 2.0f;
				float angle_lat = float(vi) / float(SAMPLES_LAT - 1) * glm::pi<float>() - glm::pi<float>() / 2.0f;

				glm::mat4 transf = glm::mat4(1.0f);
				transf = glm::rotate(transf, angle_lon, glm::vec3(0.0f, 0.0f, 1.0f));
				transf = glm::rotate(transf, angle_lat, glm::vec3(0.0f, 1.0f, 0.0f));
				transf = glm::translate(transf, glm::vec3(radius, 0.0f, 0.0f));
				glm::vec4 opt = transf * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				glm::vec3 pt = glm::vec3(opt) / opt.w;

				glm::vec3 normal = glm::normalize(pt);

				m_vertices[vertex_counter * 3 + 0] = pt.x;
				m_vertices[vertex_counter * 3 + 1] = pt.y;
				m_vertices[vertex_counter * 3 + 2] = pt.z;

				m_colors[vertex_counter * 3 + 0] = color.r;
				m_colors[vertex_counter * 3 + 1] = color.g;
				m_colors[vertex_counter * 3 + 2] = color.b;

				m_normals[vertex_counter * 3 + 0] = normal.x;
				m_normals[vertex_counter * 3 + 1] = normal.y;
				m_normals[vertex_counter * 3 + 2] = normal.z;
				vertex_counter++;
			}

		// bottom vertex
		m_vertices[vertex_counter * 3 + 0] = 0.0f;
		m_vertices[vertex_counter * 3 + 1] = 0.0f;
		m_vertices[vertex_counter * 3 + 2] = -radius;

		m_colors[vertex_counter * 3 + 0] = color.r;
		m_colors[vertex_counter * 3 + 1] = color.g;
		m_colors[vertex_counter * 3 + 2] = color.b;

		m_normals[vertex_counter * 3 + 0] = 0.0f;
		m_normals[vertex_counter * 3 + 1] = 0.0f;
		m_normals[vertex_counter * 3 + 2] = -1.0f;
		int bottom_vertex_index = vertex_counter;
		vertex_counter++;

		// top vertex
		m_vertices[vertex_counter * 3 + 0] = 0.0f;
		m_vertices[vertex_counter * 3 + 1] = 0.0f;
		m_vertices[vertex_counter * 3 + 2] = radius;

		m_colors[vertex_counter * 3 + 0] = color.r;
		m_colors[vertex_counter * 3 + 1] = color.g;
		m_colors[vertex_counter * 3 + 2] = color.b;

		m_normals[vertex_counter * 3 + 0] = 0.0f;
		m_normals[vertex_counter * 3 + 1] = 0.0f;
		m_normals[vertex_counter * 3 + 2] = 1.0f;
		int top_vertex_index = vertex_counter;
		vertex_counter++;

		// generate faces
		// top triangles
		for (int hi = 0; hi < SAMPLES_LON; hi++)
		{
			m_faces[face_counter * 3 + 0] = top_vertex_index;
			m_faces[face_counter * 3 + 1] = hi;
			m_faces[face_counter * 3 + 2] = (hi + 1) % SAMPLES_LON;
			face_counter++;
		}

		// quad faces
		for (int vi = 1; vi < SAMPLES_LAT - 2; vi++)
			for (int hi = 0; hi < SAMPLES_LON; hi++)
			{
				m_faces[face_counter * 3 + 0] = (vi - 1) * SAMPLES_LON + hi;
				m_faces[face_counter * 3 + 1] = (vi + 0) * SAMPLES_LON + hi;
				m_faces[face_counter * 3 + 2] = (vi + 0) * SAMPLES_LON + (hi + 1) % SAMPLES_LON;
				face_counter++;

				m_faces[face_counter * 3 + 0] = (vi - 1) * SAMPLES_LON + hi;
				m_faces[face_counter * 3 + 1] = (vi + 0) * SAMPLES_LON + (hi + 1) % SAMPLES_LON;
				m_faces[face_counter * 3 + 2] = (vi - 1) * SAMPLES_LON + (hi + 1) % SAMPLES_LON;
				face_counter++;
			}

		// bottom triangles
		for (int hi = 0; hi < SAMPLES_LON; hi++)
		{
			m_faces[face_counter * 3 + 0] = (SAMPLES_LAT - 3) * SAMPLES_LON + hi;
			m_faces[face_counter * 3 + 1] = bottom_vertex_index;
			m_faces[face_counter * 3 + 2] = (SAMPLES_LAT - 3) * SAMPLES_LON + (hi + 1) % SAMPLES_LON;
			face_counter++;
		}

		m_faces_size = face_counter;
		m_vertices_size = vertex_counter;
	}

	~SphereGeometry()
	{
		delete[] m_vertices;
		delete[] m_colors;
		delete[] m_faces;
		delete[] m_normals;
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

    PrismGeometry mesh;
    ModelRenderer triangle_renderer(mesh);
    renderer = &triangle_renderer;

	ConeGeometry cone_geo(2.0, 4.0, glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	ModelRenderer cone_geo_renderer(cone_geo);
	cone_renderer = &cone_geo_renderer;

	SphereGeometry geo(2.0, glm::vec3(0.0, 1.0, 0.0));
	ModelRenderer geo_renderer(geo);
	sphere_renderer = &geo_renderer;

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

    colorTextureUniformLocation = glGetUniformLocation(shaderProgram, "color_texture");
	hasTextureUniformLocation = glGetUniformLocation(shaderProgram, "has_texture");

    // texture
    colorTexture = LoadTexture("src/textures/p1_granito.png", GL_MIRRORED_REPEAT, GL_LINEAR);

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