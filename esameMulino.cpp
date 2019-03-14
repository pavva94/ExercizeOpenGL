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

unsigned int scr_width = 600*3;
unsigned int scr_height = 600*2;

int shaderProgram;
GLint transformationUniformLocation;
GLint modelviewUniformLocation;

GLint lightPositionUniformLocation;
GLint lightAmbientUniformLocation;
GLint lightDiffuseUniformLocation;
GLint lightSpecularUniformLocation;

GLint shininessUniformLocation;
GLint colorSpecularUniformLocation;
GLint colorEmittedUniformLocation;
GLint hasTextureUniformLocation;

GLint colorTextureUniformLocation;
GLuint carTexture;
GLuint carTexture2;

ModelRenderer * piramid_renderer;
ModelRenderer * cube_renderer;
ModelRenderer * cube_rendererV;
ModelRenderer * pala_renderer;
ModelRenderer * car_renderer;
ModelRenderer * ruota_renderer;
ModelRenderer * sfera;
ModelRenderer * corda_renderer;
ModelRenderer * barile_renderer;
ModelRenderer * sferaRosa_renderer;
ModelRenderer * triangle_renderer;

float movimentoPala = 0.0f;
float movimentoPendolo = glm::pi<float>();
float movimentoCorda = glm::pi<float>();
float ingranaggioN = 0.0f;
float ingranaggioP = 0.37f;
float rotolamento = 0.0f;
float rotolamento2 = 0.0f;
float caduta = 25.0f;
float velocit‡ = 1.0f;
float velocit‡Scivolamento = 1.0f;
float velocit‡Rotolamento = 1.0f;
float scivolamento = 1.0f;
float y = -15.0f;
float luceAmbientale = 0.3f;
float luceDiffusa= 0.7f;
float luceSpeculare = 1.0f;
float a = 0.3f;
float b = 0.3f;
float c = 0.3f;

glm::mat4 inputModelMatrix = glm::mat4(1.0);

bool segnoPositivo = true;
bool MovBasso = true;
bool add = true;
bool velocitaZero = false;
bool rampa = true;

int contatore = 0;

class CylinderGeometry : public IGeometry
{
public:
	CylinderGeometry(float radius, float height, glm::vec3 color_top, glm::vec3 color_bottom, glm::vec3 color_side)
	{
		const int SAMPLES = 40;
		m_vertices = new GLfloat[3 * 4 * SAMPLES];
		m_colors = new GLfloat[3 * 4 * SAMPLES];
		m_normals = new GLfloat[3 * 4 * SAMPLES];
		m_faces = new GLuint[3 * (2 * SAMPLES + (SAMPLES - 2) + (SAMPLES - 2))];

		int face_counter = 0;
		int vertex_counter = 0;

		int zero_vertex;
		// top face
		zero_vertex = vertex_counter;
		for (int i = 0; i < SAMPLES; i++)
		{
			const float angle = float(i) / float(SAMPLES) * glm::pi<float>() * 2.0f;
			const float x = radius * std::cos(angle);
			const float y = radius * std::sin(angle);
			const float z = height / 2.0f;

			m_vertices[vertex_counter * 3 + 0] = x;
			m_vertices[vertex_counter * 3 + 1] = y;
			m_vertices[vertex_counter * 3 + 2] = z;

			m_colors[vertex_counter * 3 + 0] = color_top.r;
			m_colors[vertex_counter * 3 + 1] = color_top.g;
			m_colors[vertex_counter * 3 + 2] = color_top.b;

			m_normals[vertex_counter * 3 + 0] = 0.0f;
			m_normals[vertex_counter * 3 + 1] = 0.0f;
			m_normals[vertex_counter * 3 + 2] = 1.0f;
			vertex_counter++;

			if (i >= 2)
			{
				m_faces[face_counter * 3 + 0] = zero_vertex;
				m_faces[face_counter * 3 + 1] = vertex_counter - 2;
				m_faces[face_counter * 3 + 2] = vertex_counter - 1;
				face_counter++;
			}
		}

		// bottom face
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

			m_colors[vertex_counter * 3 + 0] = color_bottom.r;
			m_colors[vertex_counter * 3 + 1] = color_bottom.g;
			m_colors[vertex_counter * 3 + 2] = color_bottom.b;

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

		// duplicate top and bottom points (color is different), connect them
		zero_vertex = vertex_counter;
		for (int i = 0; i < SAMPLES; i++)
		{
			const float angle = float(i) / float(SAMPLES) * glm::pi<float>() * 2.0f;
			const float x = radius * std::cos(angle);
			const float y = radius * std::sin(angle);
			const float zt = height / 2.0f;
			const float zb = -height / 2.0f;

			glm::vec3 normal(x, y, 0.0f);
			normal = glm::normalize(normal);

			m_vertices[vertex_counter * 3 + 0] = x;
			m_vertices[vertex_counter * 3 + 1] = y;
			m_vertices[vertex_counter * 3 + 2] = zt;

			m_colors[vertex_counter * 3 + 0] = color_side.r;
			m_colors[vertex_counter * 3 + 1] = color_side.g;
			m_colors[vertex_counter * 3 + 2] = color_side.b;

			m_normals[vertex_counter * 3 + 0] = normal.x;
			m_normals[vertex_counter * 3 + 1] = normal.y;
			m_normals[vertex_counter * 3 + 2] = normal.z;
			vertex_counter++;

			m_vertices[vertex_counter * 3 + 0] = x;
			m_vertices[vertex_counter * 3 + 1] = y;
			m_vertices[vertex_counter * 3 + 2] = zb;

			m_colors[vertex_counter * 3 + 0] = color_side.r;
			m_colors[vertex_counter * 3 + 1] = color_side.g;
			m_colors[vertex_counter * 3 + 2] = color_side.b;

			m_normals[vertex_counter * 3 + 0] = normal.x;
			m_normals[vertex_counter * 3 + 1] = normal.y;
			m_normals[vertex_counter * 3 + 2] = normal.z;
			vertex_counter++;

			m_faces[face_counter * 3 + 0] = zero_vertex + i * 2;
			m_faces[face_counter * 3 + 1] = zero_vertex + i * 2 + 1;
			m_faces[face_counter * 3 + 2] = zero_vertex + ((i + 1) % SAMPLES) * 2 + 1;
			face_counter++;

			m_faces[face_counter * 3 + 0] = zero_vertex + i * 2;
			m_faces[face_counter * 3 + 1] = zero_vertex + ((i + 1) % SAMPLES) * 2 + 1;
			m_faces[face_counter * 3 + 2] = zero_vertex + ((i + 1) % SAMPLES) * 2;
			face_counter++;
		}

		m_faces_size = face_counter;
		m_vertices_size = vertex_counter;
	}

	~CylinderGeometry()
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

void display(GLFWwindow* window)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);

	const float PI = std::acos(-1.0f);
	glm::mat4 model_matrix = glm::mat4(1.0);
	model_matrix = inputModelMatrix * model_matrix;

	glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-15.0f, 0.0f, -140.0f));
	glm::mat4 projection_matrix = glm::perspective(glm::pi<float>() / 4.0f, float(scr_width) / float(scr_height), 1.0f, 205.0f);

	glm::mat4 transf = projection_matrix * view_matrix * model_matrix;
	glm::mat4 modelview = view_matrix * model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

	glm::vec3 light_position(0.0f, 0.0f, 145.0f);
	glm::vec3 light_camera_position = glm::vec3(view_matrix * glm::vec4(light_position, 1.0f));
	glm::vec3 light_ambient(luceAmbientale, luceAmbientale, luceAmbientale);
	glm::vec3 light_diffuse(luceDiffusa, luceDiffusa, luceDiffusa);
	glm::vec3 light_specular(luceSpeculare, luceSpeculare, luceSpeculare);
	glUniform3fv(lightPositionUniformLocation, 1, glm::value_ptr(light_camera_position));
	glUniform3fv(lightAmbientUniformLocation, 1, glm::value_ptr(light_ambient));
	glUniform3fv(lightDiffuseUniformLocation, 1, glm::value_ptr(light_diffuse));
	glUniform3fv(lightSpecularUniformLocation, 1, glm::value_ptr(light_specular));

	GLfloat shininess = 100.0f;
	glUniform1f(shininessUniformLocation, shininess);
	glUniform3f(colorSpecularUniformLocation, 0.7f, 0.7f, 0.7f);
	glUniform3f(colorEmittedUniformLocation, 0.0f, 0.0f, 0.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carTexture);
	glUniform1i(colorTextureUniformLocation, 0);

	glUniform1i(hasTextureUniformLocation, 1);

	glm::mat4 piramid_model_matrix = model_matrix;
	piramid_model_matrix = glm::scale(piramid_model_matrix, glm::vec3(2, 2, 2));
	transf = projection_matrix * view_matrix * piramid_model_matrix;
	modelview = view_matrix * piramid_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	piramid_renderer->render();

	glUniform1i(hasTextureUniformLocation, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carTexture2);
	glUniform1i(colorTextureUniformLocation, 0);

	glUniform1i(hasTextureUniformLocation, 1);

	glm::mat4 car_model_matrix = model_matrix;
	car_model_matrix = glm::rotate(car_model_matrix, movimentoPala, glm::vec3(0, 1, 0.0f));
	car_model_matrix = glm::translate(car_model_matrix, glm::vec3(22.0, -20.0, 0.0));
	car_model_matrix = glm::rotate(car_model_matrix, -glm::pi<float>() / 2.0f, glm::vec3(1, 0, 0.0f));
	transf = projection_matrix * view_matrix * car_model_matrix;
	modelview = view_matrix * car_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

	car_renderer->render();

	glm::mat4 car2_model_matrix = model_matrix;
	car2_model_matrix = glm::rotate(car2_model_matrix, movimentoPala *1.2f, glm::vec3(0, 1, 0.0f));
	car2_model_matrix = glm::translate(car2_model_matrix, glm::vec3(27.0, -20.0, 0.0));
	car2_model_matrix = glm::rotate(car2_model_matrix, -glm::pi<float>() / 2.0f, glm::vec3(1, 0, 0.0f));
	transf = projection_matrix * view_matrix * car2_model_matrix;
	modelview = view_matrix * car2_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	car_renderer->render();

	glUniform1i(hasTextureUniformLocation, 0);

	glm::mat4 cube_model_matrix = model_matrix;
	cube_model_matrix = glm::translate(cube_model_matrix, glm::vec3(0.0, 8.0, 5.0));
	cube_model_matrix = glm::rotate(cube_model_matrix, movimentoPala, glm::vec3(0.0f, 0.0f, 1.0f));
	cube_model_matrix = glm::scale(cube_model_matrix, glm::vec3(1, 1, 4));
	transf = projection_matrix * view_matrix * cube_model_matrix;
	modelview = view_matrix * cube_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	cube_renderer->render();

	glm::mat4 cube2_model_matrix = model_matrix;
	cube2_model_matrix = glm::translate(cube2_model_matrix, glm::vec3(0.0, 8.0, -9.0));
	cube2_model_matrix = glm::scale(cube2_model_matrix, glm::vec3(2.0f, 2.0f, 8.0f));
	transf = projection_matrix * view_matrix * cube2_model_matrix;
	modelview = view_matrix * cube2_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	cube_renderer->render();

	glm::mat4 pale_model_matrix = model_matrix;
	pale_model_matrix = glm::translate(pale_model_matrix, glm::vec3(0.0f, 8.0f, 8.5f));
	pale_model_matrix = glm::rotate(pale_model_matrix, movimentoPala, glm::vec3(0.0f, 0.0f, 1.0f));
	pale_model_matrix = glm::scale(pale_model_matrix, glm::vec3(0.5f, 0.5f, 0.1f));

	glUniform1i(hasTextureUniformLocation, 0);

	glm::mat4 pala1_model_matrix = pale_model_matrix;
	transf = projection_matrix * view_matrix * pala1_model_matrix;
	modelview = view_matrix * pala1_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	pala_renderer->render();

	glm::mat4 pala2_model_matrix = pale_model_matrix;
	pala2_model_matrix = glm::rotate(pala2_model_matrix, glm::pi<float>() / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	transf = projection_matrix * view_matrix * pala2_model_matrix;
	modelview = view_matrix * pala2_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	pala_renderer->render();

	glm::mat4 pala3_model_matrix = pale_model_matrix;
	pala3_model_matrix = glm::rotate(pala3_model_matrix, glm::pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
	transf = projection_matrix * view_matrix * pala3_model_matrix;
	modelview = view_matrix * pala3_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	pala_renderer->render();

	glm::mat4 pala4_model_matrix = pale_model_matrix;
	pala4_model_matrix = glm::rotate(pala4_model_matrix, -glm::pi<float>() / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	transf = projection_matrix * view_matrix * pala4_model_matrix;
	modelview = view_matrix * pala4_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	pala_renderer->render();

	glm::mat4 pala5_model_matrix = pale_model_matrix;
	pala5_model_matrix = glm::rotate(pala5_model_matrix, -glm::pi<float>() / 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	transf = projection_matrix * view_matrix * pala5_model_matrix;
	modelview = view_matrix * pala5_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	pala_renderer->render();

	glm::mat4 pala6_model_matrix = pale_model_matrix;
	pala6_model_matrix = glm::rotate(pala6_model_matrix, 3.0f * glm::pi<float>() / 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	transf = projection_matrix * view_matrix * pala6_model_matrix;
	modelview = view_matrix * pala6_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	pala_renderer->render();

	glm::mat4 pala7_model_matrix = pale_model_matrix;
	pala7_model_matrix = glm::rotate(pala7_model_matrix, glm::pi<float>() / 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	transf = projection_matrix * view_matrix * pala7_model_matrix;
	modelview = view_matrix * pala7_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	pala_renderer->render();

	glm::mat4 pala8_model_matrix = pale_model_matrix;
	pala8_model_matrix = glm::rotate(pala8_model_matrix, 3.0f * -glm::pi<float>() / 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	transf = projection_matrix * view_matrix * pala8_model_matrix;
	modelview = view_matrix * pala8_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	pala_renderer->render();

	glm::mat4 sfera1 = model_matrix;
	sfera1 = glm::translate(sfera1, glm::vec3(0.0, 8.0f, -15.8f));
	sfera1 = glm::rotate(sfera1, movimentoPendolo, glm::vec3(0, 0, 1));
	sfera1 = glm::translate(sfera1, glm::vec3(0.0, 6.0f, 0.0f));
	sfera1 = glm::scale(sfera1, glm::vec3(2, 2, 2));
	transf = projection_matrix * view_matrix * sfera1;
	modelview = view_matrix * sfera1;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	sfera->render();

	glm::mat4 corda1 = model_matrix;
	corda1 = glm::translate(corda1, glm::vec3(0.0f, 6.5f, -15.8f));
	corda1 = glm::rotate(corda1, movimentoCorda, glm::vec3(0, 0, 1));
	corda1 = glm::translate(corda1, glm::vec3(0.0, 2.0f, 0.0f));
	corda1 = glm::rotate(corda1, glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	corda1 = glm::scale(corda1, glm::vec3(0.5f, 0.1f, 2.0f));
	transf = projection_matrix * view_matrix * corda1;
	modelview = view_matrix * corda1;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	corda_renderer->render();

	for (int i = 0; i < 2; ++i) {

		glm::mat4 ruota_model_matrix = model_matrix;

		ruota_model_matrix = glm::translate(ruota_model_matrix, glm::vec3(15.0f * float(i), -15.3f * (float)((i) % 2), 0.0f));
		ruota_model_matrix = glm::translate(ruota_model_matrix, glm::vec3(25.0, 8.0f, 0.0f));

		if (i % 2 == 0)
			ruota_model_matrix = glm::rotate(ruota_model_matrix, ingranaggioN, glm::vec3(0.0f, 0.0f, 1.0f));
		else
			ruota_model_matrix = glm::rotate(ruota_model_matrix, ingranaggioP, glm::vec3(0.0f, 0.0f, 1.0f));

		ruota_model_matrix = glm::scale(ruota_model_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		transf = projection_matrix * view_matrix * ruota_model_matrix;
		modelview = view_matrix * ruota_model_matrix;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		ruota_renderer->render();

		glm::mat4 dente_model_matrix = ruota_model_matrix;
		dente_model_matrix = glm::translate(dente_model_matrix, glm::vec3(0.0f, -5.0f, 0.0f));
		dente_model_matrix = glm::scale(dente_model_matrix, glm::vec3(1.0f, 0.6f, 0.45f));
		transf = projection_matrix * view_matrix * dente_model_matrix;
		modelview = view_matrix * dente_model_matrix;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		cube_rendererV->render();

		glm::mat4 dente_model_matrix2 = ruota_model_matrix;
		dente_model_matrix2 = glm::translate(dente_model_matrix2, glm::vec3(0.0f, 5.0f, 0.0f));
		dente_model_matrix2 = glm::scale(dente_model_matrix2, glm::vec3(1.0f, 0.6f, 0.45f));
		transf = projection_matrix * view_matrix * dente_model_matrix2;
		modelview = view_matrix * dente_model_matrix2;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		cube_rendererV->render();

		glm::mat4 dente_model_matrix3 = ruota_model_matrix;
		dente_model_matrix3 = glm::translate(dente_model_matrix3, glm::vec3(5.0f, 0.0f, 0.0f));
		dente_model_matrix3 = glm::rotate(dente_model_matrix3, glm::pi<float>() / 2.0f, glm::vec3(0, 0, 1));
		dente_model_matrix3 = glm::scale(dente_model_matrix3, glm::vec3(1.0f, 0.6f, 0.45f));
		transf = projection_matrix * view_matrix * dente_model_matrix3;
		modelview = view_matrix * dente_model_matrix3;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		cube_rendererV->render();

		glm::mat4 dente_model_matrix4 = ruota_model_matrix;
		dente_model_matrix4 = glm::translate(dente_model_matrix4, glm::vec3(-5.0f, 0.0f, 0.0f));
		dente_model_matrix4 = glm::rotate(dente_model_matrix4, glm::pi<float>() / 2.0f, glm::vec3(0, 0, 1));
		dente_model_matrix4 = glm::scale(dente_model_matrix4, glm::vec3(1.0f, 0.6f, 0.45f));
		transf = projection_matrix * view_matrix * dente_model_matrix4;
		modelview = view_matrix * dente_model_matrix4;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		cube_rendererV->render();

		glm::mat4 dente_model_matrix5 = ruota_model_matrix;
		dente_model_matrix5 = glm::translate(dente_model_matrix5, glm::vec3(-3.5f, -3.5f, 0.0f));
		dente_model_matrix5 = glm::rotate(dente_model_matrix5, -glm::pi<float>() / 4.0f, glm::vec3(0, 0, 1));
		dente_model_matrix5 = glm::scale(dente_model_matrix5, glm::vec3(1.0f, 0.6f, 0.45f));
		transf = projection_matrix * view_matrix * dente_model_matrix5;
		modelview = view_matrix * dente_model_matrix5;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		cube_rendererV->render();

		glm::mat4 dente_model_matrix6 = ruota_model_matrix;
		dente_model_matrix6 = glm::translate(dente_model_matrix6, glm::vec3(3.5f, 3.5f, 0.0f));
		dente_model_matrix6 = glm::rotate(dente_model_matrix6, -glm::pi<float>() / 4.0f, glm::vec3(0, 0, 1));
		dente_model_matrix6 = glm::scale(dente_model_matrix6, glm::vec3(1.0f, 0.6f, 0.45f));
		transf = projection_matrix * view_matrix * dente_model_matrix6;
		modelview = view_matrix * dente_model_matrix6;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		cube_rendererV->render();

		glm::mat4 dente_model_matrix7 = ruota_model_matrix;
		dente_model_matrix7 = glm::translate(dente_model_matrix7, glm::vec3(-3.5f, 3.5f, 0.0f));
		dente_model_matrix7 = glm::rotate(dente_model_matrix7, glm::pi<float>() / 4.0f, glm::vec3(0, 0, 1));
		dente_model_matrix7 = glm::scale(dente_model_matrix7, glm::vec3(1.0f, 0.6f, 0.45f));
		transf = projection_matrix * view_matrix * dente_model_matrix7;
		modelview = view_matrix * dente_model_matrix7;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		cube_rendererV->render();

		glm::mat4 dente_model_matrix8 = ruota_model_matrix;
		dente_model_matrix8 = glm::translate(dente_model_matrix8, glm::vec3(3.5f, -3.5f, 0.0f));
		dente_model_matrix8 = glm::rotate(dente_model_matrix8, glm::pi<float>() / 4.0f, glm::vec3(0, 0, 1));
		dente_model_matrix8 = glm::scale(dente_model_matrix8, glm::vec3(1.0f, 0.6f, 0.45f));
		transf = projection_matrix * view_matrix * dente_model_matrix8;
		modelview = view_matrix * dente_model_matrix8;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		cube_rendererV->render();

	}

	CylinderGeometry geo4(2.0f, 5.0f, glm::vec3(a, b, c), glm::vec3(a, b, c), glm::vec3(a, b, c));
	ModelRenderer geo_renderer4(geo4);
	barile_renderer = &geo_renderer4;

	if (contatore > 200) {
		a = (float)(rand() % 256) / 256.0f;
		b = (float)(rand() % 256) / 256.0f;
		c = (float)(rand() % 256) / 256.0f;
		std::cout << a << std::endl;
		contatore = 0;
		
	}
	contatore++;
	shininess = 100.0f;
	glUniform1f(shininessUniformLocation, shininess);
	glUniform3f(colorSpecularUniformLocation, 1.0f, 0.0f, 0.f);
	glUniform3f(colorEmittedUniformLocation, a, a, a);

	glm::mat4 barile_model_matrix = model_matrix;
	barile_model_matrix = glm::translate(barile_model_matrix, glm::vec3(-35.0f, -19.0f, 40.0f));
	barile_model_matrix = glm::translate(barile_model_matrix, glm::vec3(-rotolamento * 2.0f, -0.0f, 0.0f));
	barile_model_matrix = glm::rotate(barile_model_matrix, rotolamento, glm::vec3(0, 0, 1));
	transf = projection_matrix * view_matrix * barile_model_matrix;
	modelview = view_matrix * barile_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	barile_renderer->render();

	shininess = 100.0f;
	glUniform1f(shininessUniformLocation, shininess);
	glUniform3f(colorSpecularUniformLocation, 0.7f, 0.7f, 0.7f);
	glUniform3f(colorEmittedUniformLocation, 0.0f, 0.0f, 0.0f);

	glm::mat4 palla_model_matrix = model_matrix;
	palla_model_matrix = glm::translate(palla_model_matrix, glm::vec3(-45.0f, 0.0f, 0.0f));
	palla_model_matrix = glm::translate(palla_model_matrix, glm::vec3(0.0f, caduta, 0.0f));
	transf = projection_matrix * view_matrix * palla_model_matrix;
	modelview = view_matrix * palla_model_matrix;
	glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
	glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
	sferaRosa_renderer->render();

	if (add) {

		glm::mat4 palla_model_matrix2 = palla_model_matrix;
		palla_model_matrix2 = glm::rotate(palla_model_matrix2, ingranaggioP * 3.0f, glm::vec3(0.0f, 1.0f, 1.0f));
		palla_model_matrix2 = glm::translate(palla_model_matrix2, glm::vec3(5.0f, 0.0f, 0.0f));
		palla_model_matrix2 = glm::scale(palla_model_matrix2, glm::vec3(2.0f, 2.0f, 2.0f));
		transf = projection_matrix * view_matrix * palla_model_matrix2;
		modelview = view_matrix * palla_model_matrix2;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		sfera->render();

		glm::mat4 palla_model_matrix3 = palla_model_matrix;
		palla_model_matrix3 = glm::rotate(palla_model_matrix3, ingranaggioP * 2.0f, glm::vec3(1.0f, 0.0f, 1.0f));
		palla_model_matrix3 = glm::translate(palla_model_matrix3, glm::vec3(9.0f, 0.0f, 0.0f));
		palla_model_matrix3 = glm::scale(palla_model_matrix3, glm::vec3(2.5f, 2.5f, 2.5f));
		transf = projection_matrix * view_matrix * palla_model_matrix3;
		modelview = view_matrix * palla_model_matrix3;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		sfera->render();

		glm::mat4 palla_model_matrix4 = palla_model_matrix;
		palla_model_matrix4 = glm::rotate(palla_model_matrix4, ingranaggioN, glm::vec3(0.0f, 0.5f, 1.0f));
		palla_model_matrix4 = glm::translate(palla_model_matrix4, glm::vec3(12.0f, 0.0f, 0.0f));
		palla_model_matrix4 = glm::scale(palla_model_matrix4, glm::vec3(0.5f, 0.5f, 0.5f));
		transf = projection_matrix * view_matrix * palla_model_matrix4;
		modelview = view_matrix * palla_model_matrix4;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		sferaRosa_renderer->render();

		glm::mat4 rampa_model_matrix = model_matrix;
		rampa_model_matrix = glm::translate(rampa_model_matrix, glm::vec3(-18.0f, -20.0f, 60.0f));
		rampa_model_matrix = glm::scale(rampa_model_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
		transf = projection_matrix * view_matrix * rampa_model_matrix;
		modelview = view_matrix * rampa_model_matrix;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		triangle_renderer->render();


		glm::mat4 barile_model_matrix2 = model_matrix;
		barile_model_matrix2 = glm::translate(barile_model_matrix2, glm::vec3(-27.0f, -13.0f, 60.0f));
		barile_model_matrix2 = glm::translate(barile_model_matrix2, glm::vec3(scivolamento, y, 0.0f));
		barile_model_matrix2 = glm::rotate(barile_model_matrix2, rotolamento2, glm::vec3(0, 0, 1));
		transf = projection_matrix * view_matrix * barile_model_matrix2;
		modelview = view_matrix * barile_model_matrix2;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));
		barile_renderer->render();

	}

	glfwSwapBuffers(window);
}
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

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		luceAmbientale -= 0.01;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		luceAmbientale += 0.01;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		luceDiffusa += 0.01;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		luceDiffusa -= 0.01;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		luceSpeculare += 0.01;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		luceSpeculare -= 0.01;

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
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		add = false;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		add = true;

	delta_y *= speed * float(time_diff);
	delta_x *= speed * float(time_diff);

	glm::mat4 rot = glm::mat4(1.0);
	rot = glm::rotate(rot, delta_x, glm::vec3(0.0, 1.0, 0.0));
	rot = glm::rotate(rot, delta_y, glm::vec3(1.0, 0.0, 0.0));
	inputModelMatrix = rot * inputModelMatrix;

	movimentoPala += glm::pi<float>() / 2.0f * (float)time_diff;

	ingranaggioN -= glm::pi<float>() / 2.0f * (float)time_diff;

	ingranaggioP += glm::pi<float>() / 2.0f * (float)time_diff;

	rotolamento -= glm::pi<float>() / 2.0f * (float)time_diff;
	


	if (!velocitaZero) {
		if (MovBasso) {
			caduta -= (float)time_diff * velocit‡;
			velocit‡ *= 1.05f;
			//std::cout << "Caduta: " << caduta << std::endl;
			//std::cout << "Velocit‡: " << velocit‡ << std::endl;
			if (caduta <= -14.0f)
				MovBasso = false;
			//if (velocit‡ < 0.5f)
			//	velocitaZero = true;
		}

		else {
			caduta += (float)time_diff * velocit‡;
			velocit‡ *= 0.935f;
			if (velocit‡ <= 1.5f)
				MovBasso = true;
		}
		
	}

	if (rampa) {
		// formula traiettoria caduta barile
		y = -0.5f*scivolamento;
		scivolamento += (float)time_diff * velocit‡Scivolamento;
		rotolamento2 -= glm::pi<float>() / 2.0f * (float)time_diff * velocit‡Rotolamento;
		velocit‡Scivolamento *= 1.035f;
		velocit‡Rotolamento *= 1.005f;
		if (y <= -10)
			rampa = false;
	}
	else {
		scivolamento += (float)time_diff * velocit‡Scivolamento;
		rotolamento2 -= glm::pi<float>() / 2.0f * (float)time_diff * velocit‡Rotolamento;
		velocit‡Scivolamento *= 0.985f;
		velocit‡Rotolamento *= 0.985f;
	}
	


	if (segnoPositivo) {
		movimentoPendolo += glm::pi<float>() * (float)time_diff;
		movimentoCorda += glm::pi<float>() * (float)time_diff * 1.15f;
		if (movimentoPendolo >= (1.5f) * glm::pi<float>()) 
			segnoPositivo = false;
	}
	else {
		movimentoPendolo -= glm::pi<float>() * (float)time_diff;
		movimentoCorda -= glm::pi<float>() * (float)time_diff * 1.15f;
		if (movimentoPendolo <= glm::pi<float>() / 2.0f)
			segnoPositivo = true;
	}

}

class PiramidGeometry : public IGeometry
{
public:
	PiramidGeometry()
	{
	}

	~PiramidGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			//front
			-5.0f, -10.0f, 5.0f,
			5.0f, -10.0f, 5.0f,
			0.0f, 10.0f, 0.0f,
			//right
			5.0f, -10.0f, 5.0f,
			5.0f, -10.0f, -5.0f,
			0.0f, 10.0f, 0.0f,
			//back
			5.0f, -10.0f, -5.0f,
			-5.0f, -10.0f, -5.0f, //7
			0.0f, 10.0f, 0.0f,
			//left
			-5.0f, -10.0f, -5.0f,
			-5.0f, -10.0f, 5.0f,
			0.0f, 10.0f, 0.0f,
			//base
			-5.0f, -10.0f, 5.0f,
			5.0f, -10.0f, 5.0f,
			5.0f, -10.0f, -5.0f,
			-5.0f, -10.0f, -5.0f,
		};

		return avertices;
	}
	const GLfloat * normals()
	{
		static GLfloat anormals[] = {
			//front
			normalFront.x, normalFront.y, normalFront.z,
			normalFront.x, normalFront.y, normalFront.z,
			normalFront.x, normalFront.y, normalFront.z,
			// right
			normalRight.x, normalRight.y, normalRight.z,
			normalRight.x, normalRight.y, normalRight.z,
			normalRight.x, normalRight.y, normalRight.z,
			//back
			normalBack.x, normalBack.y, normalBack.z,
			normalBack.x, normalBack.y, normalBack.z,
			normalBack.x, normalBack.y, normalBack.z,
			//left
			normalLeft.x, normalLeft.y, normalLeft.z,
			normalLeft.x, normalLeft.y, normalLeft.z,
			normalLeft.x, normalLeft.y, normalLeft.z,
			
			 //base
			 0.0,   -1.0,   0.0,
			 0.0,   -1.0,   0.0,
			 0.0,   -1.0,   0.0,
			 0.0,   -1.0,   0.0,
		};

		return anormals;
	}
	
	GLsizei verticesSize() { return 16; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2,
			3, 4, 5,
			6, 7, 8, 
			9, 10, 11,
			15, 14, 13,
			15, 13, 12,
		};

		return indices;
	}
	GLsizei size() { return 18; }

	/*const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
			0.5,   0.5,   0.5,
		};

		return acolors;
	}*/
	const GLfloat * texCoords()
	{
		static GLfloat atexcoords[] = {
			0.5f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,

			0.5f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,

			0.5f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,

			0.5f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,



		};

		return atexcoords;
	}


	GLenum type() { return GL_TRIANGLES; }

private:

	glm::vec3 normalFront = computeNormal(glm::vec3(-5.0f, -10.0f, 5.0f), glm::vec3(5.0f, -10.0f, 5.0f), glm::vec3(0.0f, 10.0f, 0.0f));
	glm::vec3 normalLeft = computeNormal(glm::vec3(-5.0f, -10.0f, -5.0f), glm::vec3(-5.0f, -10.0f, 5.0f), glm::vec3(0.0f, 10.0f, 0.0f));
	glm::vec3 normalRight = computeNormal(glm::vec3(5.0f, -10.0f, 5.0f), glm::vec3(5.0f, -10.0f, -5.0f), glm::vec3(0.0f, 10.0f, 0.0f));
	glm::vec3 normalBack = computeNormal(glm::vec3(5.0f, -10.0f, -5.0f), glm::vec3(-5.0f, -10.0f, -5.0f), glm::vec3(0.0f, 10.0f, 0.0f));
};

class CubeGeometry : public IGeometry
{
public:
	CubeGeometry()
	{
	}

	~CubeGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			-1.0,  -1.0,   1.0,
			 1.0,  -1.0,   1.0,
			 1.0,   1.0,   1.0,
			-1.0,   1.0,   1.0,

			-1.0,  -1.0,  -1.0,
			 1.0,  -1.0,  -1.0,
			 1.0,   1.0,  -1.0,
			-1.0,   1.0,  -1.0,
		};

		return avertices;
	}
	const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,

			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
		};

		return acolors;
	}
	GLsizei verticesSize() { return 8; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0, 1, 2, // front
			0, 2, 3, // front

			0, 3, 4, // left
			4, 3, 7, // left

			1, 5, 2, // right
			2, 5, 6, // right

			3, 2, 7, // top
			2, 6, 7, // top

			0, 4, 1, // bottom
			4, 5, 1, // bottom

			4, 7, 5, // back
			5, 7, 6, // back
		};

		return indices;
	}
	GLsizei size() { return 36; }

	GLenum type() { return GL_TRIANGLES; }

private:
};

class CubeGeometryV : public IGeometry
{
public:
	CubeGeometryV()
	{
	}

	~CubeGeometryV()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			-1.0,  -1.0,   1.0,
			 1.0,  -1.0,   1.0,
			 1.0,   1.0,   1.0,
			-1.0,   1.0,   1.0,

			-1.0,  -1.0,  -1.0,
			 1.0,  -1.0,  -1.0,
			 1.0,   1.0,  -1.0,
			-1.0,   1.0,  -1.0,
		};

		return avertices;
	}
	const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			1.0f,  1.0f,   1.0f,
			1.0f,  1.0f,   1.0f,
			1.0f,  1.0f,   1.0f,
			1.0f,  1.0f,   1.0f,

			1.0f,  1.0f,   1.0f,
			1.0f,  1.0f,   1.0f,
			1.0f,  1.0f,   1.0f,
			1.0f,  1.0f,   1.0f,
		};

		return acolors;
	}
	GLsizei verticesSize() { return 8; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0, 1, 2, // front
			0, 2, 3, // front

			0, 3, 4, // left
			4, 3, 7, // left

			1, 5, 2, // right
			2, 5, 6, // right

			3, 2, 7, // top
			2, 6, 7, // top

			0, 4, 1, // bottom
			4, 5, 1, // bottom

			4, 7, 5, // back
			5, 7, 6, // back
		};

		return indices;
	}
	GLsizei size() { return 36; }

	GLenum type() { return GL_TRIANGLES; }

private:
};

class PalaGeometry : public IGeometry
{
public:
	PalaGeometry()
	{
	}

	~PalaGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			//front
			-5.0f, -20.0f, 5.0f,
			5.0f, -20.0f, 5.0f,
			0.0f, 0.0f, 0.0f,
			//right
			5.0f, -20.0f, 5.0f,
			5.0f, -20.0f, -5.0f,
			0.0f, 0.0f, 0.0f,
			//back
			5.0f, -20.0f, -5.0f,
			-5.0f, -20.0f, -5.0f, //7
			0.0f, 0.0f, 0.0f,
			//left
			-5.0f, -20.0f, -5.0f,
			-5.0f, -20.0f, 5.0f,
			0.0f, 0.0f, 0.0f,
			//base
			-5.0f, -20.0f, 5.0f,
			5.0f, -20.0f, 5.0f,
			5.0f, -20.0f, -5.0f,
			-5.0f, -20.0f, -5.0f,
		};

		return avertices;
	}
	const GLfloat * normals()
	{
		static GLfloat anormals[] = {
			//front
			normalFront.x, normalFront.y, normalFront.z,
			normalFront.x, normalFront.y, normalFront.z,
			normalFront.x, normalFront.y, normalFront.z,
			// right
			normalRight.x, normalRight.y, normalRight.z,
			normalRight.x, normalRight.y, normalRight.z,
			normalRight.x, normalRight.y, normalRight.z,
			//back
			normalBack.x, normalBack.y, normalBack.z,
			normalBack.x, normalBack.y, normalBack.z,
			normalBack.x, normalBack.y, normalBack.z,
			//left
			normalLeft.x, normalLeft.y, normalLeft.z,
			normalLeft.x, normalLeft.y, normalLeft.z,
			normalLeft.x, normalLeft.y, normalLeft.z,

			//base
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
			0.0,   -1.0,   0.0,
		};

		return anormals;
	}

	GLsizei verticesSize() { return 16; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0,  1,  2,
			3, 4, 5,
			6, 7, 8,
			9, 10, 11,
			15, 14, 13,
			15, 13, 12,
		};

		return indices;
	}
	GLsizei size() { return 18; }

	const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
			0.45,  0.16,   0.17,
		};

		return acolors;
	}

	GLenum type() { return GL_TRIANGLES; }

private:

	glm::vec3 normalFront = computeNormal(glm::vec3(-5.0f, -20.0f, 5.0f), glm::vec3(5.0f, -20.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::vec3 normalLeft = computeNormal(glm::vec3(-5.0f, -20.0f, -5.0f), glm::vec3(-5.0f, -20.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::vec3 normalRight = computeNormal(glm::vec3(5.0f, -20.0f, 5.0f), glm::vec3(5.0f, -20.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::vec3 normalBack = computeNormal(glm::vec3(5.0f, -20.0f, -5.0f), glm::vec3(-5.0f, -20.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f));
};

class SphereGeometry : public IGeometry
{
public:
	SphereGeometry(float radius, glm::vec3 color)
	{
		const int SAMPLES_LAT = 32;
		const int SAMPLES_LON = 32;
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
				glm::vec4 opt = transf * glm::vec4(radius, 0.0f, 0.0f, 1.0f);
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



class TriangleGeometry : public IGeometry
{
public:
	TriangleGeometry()
	{
	}

	~TriangleGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			// front
			-5.0, -2.5,  5.0, // 0
			 5.0, -2.5,  5.0, // 1
			-5.0,  2.5,  5.0, // 2
			// top
			5.0, -2.5,  5.0, // 3
			5.0, -2.5, -5.0, // 4
		   -5.0,  2.5, -5.0, // 5
		   -5.0,  2.5,  5.0, // 6
		   // back
			5.0, -2.5, -5.0, // 7
		   -5.0, -2.5, -5.0, // 8
		   -5.0,  2.5, -5.0, // 9
		   // left
		   -5.0, -2.5,  5.0, // 10
		   -5.0, -2.5, -5.0, // 11
		   -5.0,  2.5, -5.0, // 12
		   -5.0,  2.5,  5.0, // 13
		   // bottom
		   -5.0, -2.5,  5.0, // 14
			5.0, -2.5,  5.0, // 15
		    5.0, -2.5, -5.0, // 16
		   -5.0, -2.5, -5.0, // 17

		};

		return avertices;
	}
	const GLfloat * normals()
	{
		static GLfloat anormals[] = {

			0, 0, 1,
			0, 0, 1,
			0, 0, 1,
			//top
			normalTop1.x, normalTop1.y, normalTop1.z,
			normalTop1.x, normalTop1.y, normalTop1.z,
			normalTop1.x, normalTop1.y, normalTop1.z,
			normalTop1.x, normalTop1.y, normalTop1.z, // 6
	
			0, 0, -1,
			0, 0, -1,
			0, 0, -1,

		   -1, 0,  0,
		   -1, 0,  0,
		   -1, 0,  0,
		   -1, 0,  0,

			0, -1, 0,
			0, -1, 0,
			0, -1, 0,
			0, -1, 0,
		};

		return anormals;
	}
	const GLfloat * colors()
	{
		static GLfloat acolors[] = {
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,

			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,

			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,

			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
			0.65,  0.16,   0.17,
		};

		return acolors;
	}
	GLsizei verticesSize() { return 18; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			// front
			0, 1, 2,
			// top
			3, 4, 5,
			3, 5, 6,
			// back
			7, 8, 9,
			// left
			11, 14, 13,
			11, 13, 12,
			// bottom
			17, 16, 15,
			17, 15, 14,
		};

		return indices;
	}
	GLsizei size() { return 24; }

	GLenum type() { return GL_TRIANGLES; }
	private:
		glm::vec3 normalTop1 = computeNormal(glm::vec3(5.0, -2.5, 5.0), glm::vec3(5.0, -2.5, -5.0), glm::vec3(-5.0, 2.5, -5.0));
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	PiramidGeometry piramid_geo;
	ModelRenderer piramid_geo_renderer(piramid_geo);
	piramid_renderer = &piramid_geo_renderer;
	
	CubeGeometry cube_geo;
	ModelRenderer cube_geo_renderer(cube_geo);
	cube_renderer = &cube_geo_renderer;

	CubeGeometryV cube_geo2;
	ModelRenderer cube_geo_renderer2(cube_geo2);
	cube_rendererV = &cube_geo_renderer2;
	
	PalaGeometry pala_geo;
	ModelRenderer pala_geo_renderer(pala_geo);
	pala_renderer = &pala_geo_renderer;

	CylinderGeometry geo3(5.0f, 1.0f, glm::vec3(0.37f, 0.37f, 0.37f), glm::vec3(0.37f, 0.37f, 0.37f), glm::vec3(0.37f, 0.37f, 0.37f));
	ModelRenderer geo_renderer3(geo3);
	ruota_renderer = &geo_renderer3;



	CylinderGeometry geo6(0.25f, 3.5f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelRenderer geo_renderer6(geo6);
	corda_renderer = &geo_renderer6;

	SphereGeometry geo2(0.6, glm::vec3(0.678, 0.804, 0.184));
	ModelRenderer geo_renderer2(geo2);
	sfera = &geo_renderer2;

	SphereGeometry geo5(3.0f, glm::vec3(1.0f, 0.0f, 1.0f));
	ModelRenderer geo_renderer5(geo5);
	sferaRosa_renderer = &geo_renderer5;

	AssimpGeometry geo("src/es10_car_texture.obj");
	ModelRenderer geo_renderer(geo);
	car_renderer = &geo_renderer;

	TriangleGeometry triangle_geo;
	ModelRenderer triangle_geo_renderer(triangle_geo);
	triangle_renderer = &triangle_geo_renderer;

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
	carTexture = LoadTexture("src/textures/wall.jpg", GL_MIRRORED_REPEAT, GL_LINEAR);

	colorTextureUniformLocation = glGetUniformLocation(shaderProgram, "color_texture");
	carTexture2 = LoadTexture("src/es10_car.png", GL_MIRRORED_REPEAT, GL_LINEAR);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

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

	glfwTerminate();
	return 0;
}