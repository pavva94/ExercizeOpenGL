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

#include "model_renderer.h"
#include "compute_normals.h"
#include "create_shader_program.h"
#include "load_texture.h"

unsigned int scr_width = 1400;
unsigned int scr_height = 1400;

int shaderProgram;
int shaderProgramSphere;
bool wireframe;
int scalini = 10;
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

ModelRenderer * prisma_renderer;
ModelRenderer * sphere_renderer;

glm::mat4 inputModelMatrix = glm::mat4(1.0); // initialize to identity
int num_gradini = 10;
float object_x = 0.0;


void display(GLFWwindow* window)
{
	// render
	// ------
	glClearColor(0.3f, 0.4f, 0.0f, 0.6f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
	else
	{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	}

	glUseProgram(shaderProgram);

	const float PI = std::acos(-1.0f);
	glm::mat4 model_matrix = glm::mat4(1.0);
	model_matrix = inputModelMatrix * model_matrix;

	glm::mat4 view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -50.0f, -400.0f));

	glm::mat4 projection_matrix = glm::perspective(glm::pi<float>() / 4.0f, float(scr_width) / float(scr_height), 0.1f, 2000.0f);

	// luci
	glm::vec3 light_position(0.0f, 200.0f, -35.0f);
	glm::vec3 light_camera_position = glm::vec3(view_matrix * glm::vec4(light_position, 1.0f));
	glm::vec3 light_ambient(0.3f, 0.3f, 0.3f);
	glm::vec3 light_diffuse(0.7f, 0.7f, 0.7f);
	glm::vec3 light_specular(1.0f, 1.0f, 1.0f);
	glUniform3fv(lightPositionUniformLocation, 1, glm::value_ptr(light_camera_position));
	glUniform3fv(lightAmbientUniformLocation, 1, glm::value_ptr(light_ambient));
	glUniform3fv(lightDiffuseUniformLocation, 1, glm::value_ptr(light_diffuse));
	glUniform3fv(lightSpecularUniformLocation, 1, glm::value_ptr(light_specular));
	// materiale
	GLfloat shininess = 100.0f;
	glUniform1f(shininessUniformLocation, shininess);
	glUniform3f(colorSpecularUniformLocation, 0.1f, 0.1f, 0.1f);
	glUniform3f(colorEmittedUniformLocation, 0.0f, 0.0f, 0.0f);
	//texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glUniform1i(colorTextureUniformLocation, 0);

	glm::mat4 transf;
	glm::mat4 modelview;

	// translate iniziale per mettere il primo gradino in alto a sinistra
	glm::mat4 prisma_model_matrix = model_matrix;
	prisma_model_matrix = glm::translate(prisma_model_matrix, glm::vec3(-120.0f, 120.0f, 0.0f));
	// scala
	for (int i = 0; i < scalini; ++i) {
		prisma_model_matrix = glm::translate(prisma_model_matrix, glm::vec3(20.0f, -20.0f, 0.0f));
		transf = projection_matrix * view_matrix * prisma_model_matrix;
		modelview = view_matrix * prisma_model_matrix;
		
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

		prisma_renderer->render();
	}

	glUseProgram(shaderProgramSphere);

	// luci
	glUniform3fv(lightPositionUniformLocation, 1, glm::value_ptr(light_camera_position));
	glUniform3fv(lightAmbientUniformLocation, 1, glm::value_ptr(light_ambient));
	glUniform3fv(lightDiffuseUniformLocation, 1, glm::value_ptr(light_diffuse));
	glUniform3fv(lightSpecularUniformLocation, 1, glm::value_ptr(light_specular));
	// materiale
	glUniform1f(shininessUniformLocation, shininess);
	glUniform3f(colorSpecularUniformLocation, 0.1f, 0.1f, 0.1f);
	glUniform3f(colorEmittedUniformLocation, 0.0f, 0.0f, 0.0f);

	glm::mat4 sphere_model_matrix = model_matrix;
	sphere_model_matrix = glm::translate(sphere_model_matrix, glm::vec3(-100.0f, 108.0f, 15.0f));

	// ciclo le 5 sfere da costruire e muovere
	for (int i = 0; i < 5; i++)
	{
		// numero gradino
		int i_gradino = object_x / 20.0;

		float f_gradino = i_gradino * 20.0;
		float x = object_x - f_gradino;
		float y = -0.2 * x * x + 3.0 * x;
		y += -20.0 * i_gradino;
		x += 20.0 * i_gradino;

		y += 5.0;

		glm::mat4 step_model_matrix = model_matrix;
		step_model_matrix = glm::translate(step_model_matrix, glm::vec3(x - 100.0, y + 100.0, i * 4.0 - 8.0));
		step_model_matrix = glm::rotate(step_model_matrix, glm::pi<float>() / 2.0f, glm::vec3(1, 0, 0));

		glm::mat4 transf = projection_matrix * view_matrix * step_model_matrix;
		glm::mat4 modelview = view_matrix * step_model_matrix;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

		sphere_renderer->render();
	}

	// creo 5 palline
	/*for (int i = 0; i < 5; ++i) {
		// movimento a 
		
		sphere_model_matrix = glm::translate(sphere_model_matrix, glm::vec3(0.0f, 0.0f, -5.0f));
		transf = projection_matrix * view_matrix * sphere_model_matrix;
		modelview = view_matrix * sphere_model_matrix;
		glUniformMatrix4fv(transformationUniformLocation, 1, GL_FALSE, glm::value_ptr(transf));
		glUniformMatrix4fv(modelviewUniformLocation, 1, GL_FALSE, glm::value_ptr(modelview));

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

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		wireframe = !wireframe;
		display(window);
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		if (scalini > 2)
			scalini -= 1;
	}

	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		if (scalini < 10)
			scalini += 1;
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

	object_x += 10.0 * time_diff;
	if (object_x > num_gradini * 20.0)
		object_x = 0.0;
}

class PrismaGeometry : public IGeometry
{
public:
	PrismaGeometry()
	{
	}

	~PrismaGeometry()
	{
	}

	const GLfloat * vertices()
	{
		static GLfloat avertices[] = {
			// faccia davanti
			0.0f, -4.0f, 15.0f,
			5.0f, 4.0f, 15.0f,
			-5.0f, 4.0f, 15.0f, 
			// faccia dietro
			0.0f, -4.0f, -15.0f,
			-5.0f, 4.0f, -15.0f, // 4
			5.0f, 4.0f, -15.0f,
			// faccia destra
			0.0f, -4.0f, 15.0f, // 6
			0.0f, -4.0f, -15.0f, // 7
			5.0f, 4.0f, -15.0f,
			5.0f, 4.0f, 15.0f,
			// faccia sopra
			5.0f, 4.0f, 15.0f, // 10
			5.0f, 4.0f, -15.0f,
			-5.0f, 4.0f, -15.0f,
			-5.0f, 4.0f, 15.0f,
			// faccia sinistra 2.0
			0.0f, -4.0f, 15.0f, // 14
			-5.0f, 4.0f, 15.0f,
			-5.0f, 4.0f, -15.0f,
			0.0f, -4.0f, -15.0f,
		};

		return avertices;
	}
	const GLfloat * normals()
	{

		
		static GLfloat anormals[] = {
			// faccia davanti
			 0.0,   0.0,   1.0,
			 0.0,   0.0,   1.0,
			 0.0,   0.0,   1.0,
			 // faccia dietro
			 0.0,   0.0,   -1.0,
			 0.0,   0.0,   -1.0,
			 0.0,   0.0,   -1.0,
			 // faccia destra
			 normals_right.x, normals_right.y, normals_right.z,
			 normals_right.x, normals_right.y, normals_right.z,
			 normals_right.x, normals_right.y, normals_right.z,
			 normals_right.x, normals_right.y, normals_right.z,
			 // faccia sopra
			 0.0, 1.0, 0.0,
			 0.0, 1.0, 0.0,
			 0.0, 1.0, 0.0,
			 0.0, 1.0, 0.0,
			 // faccia sinistra
			 normals_left.x, normals_left.y, normals_left.z,
			 normals_left.x, normals_left.y, normals_left.z,
			 normals_left.x, normals_left.y, normals_left.z,
			 normals_left.x, normals_left.y, normals_left.z,
		};

		return anormals;
	}
	const GLfloat * texCoords()
	{
		static GLfloat atexcoords[] = {
			// faccia davanti
			0.25f, 0.6f,
			0, 1, 
			0.5f, 1,
			// faccia dietro
			0.75f, 0.6f, 
			0.5f, 1,
			1, 1,
			// faccia destra
			0, 0.34f,
			1, 0.34f,
			1, 0.18f,
			0, 0.18f,
			// sopra
			0, 0.18f,
			1, 0.18f,
			1, 0,
			0, 0,
			// sinistra
			0, 0.5f,
			0, 0.34f,
			1, 0.5f,
			1, 0.34f,
		};

		return atexcoords;
	}
	GLsizei verticesSize() { return 18; }
	const GLuint * faces()
	{
		static unsigned int indices[] = {
			0, 1, 2,
			3, 4, 5,
			6, 7, 8, 
			6, 8, 9,
			10, 11, 12, 
			10, 12, 13,
			14, 15, 16, 
			14, 16, 17,

		};

		return indices;
	}
	GLsizei size() { return 24; }

	GLenum type() { return GL_TRIANGLES; }

private:
	glm::vec3 normals_left = computeNormal(glm::vec3(0.0f, -4.0f, 15.0f), glm::vec3(-5.0f, 4.0f, 15.0f), glm::vec3(-5.0f, 4.0f, -15.0f));
	glm::vec3 normals_right = computeNormal(glm::vec3(0.0f, -4.0f, 15.0f), glm::vec3(0.0f, -4.0f, -15.0f), glm::vec3(5.0f, 4.0f, -15.0f));
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

	PrismaGeometry mesh;
	ModelRenderer prisma_renderer_model(mesh);
	prisma_renderer = &prisma_renderer_model;

	SphereGeometry sph_mesh(2.0f, glm::vec3(0.19f, 1.0f, 0.19f));
	ModelRenderer sphere_renderer_model(sph_mesh);
	sphere_renderer = &sphere_renderer_model;

	// load GLSL shaders for PRISMA
	shaderProgram = createShaderProgram("es9_texture.vert", "es9_texture.frag");
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

	// texture
	colorTexture = LoadTexture("src/textures/p1_granito.png", GL_MIRRORED_REPEAT, GL_LINEAR);

	
	// load GLSL shader for SPHere
	shaderProgramSphere = createShaderProgram("es7_phong.vert", "es7_phong.frag");
	transformationUniformLocation = glGetUniformLocation(shaderProgramSphere, "transformation");
	modelviewUniformLocation = glGetUniformLocation(shaderProgramSphere, "modelview");

	lightPositionUniformLocation = glGetUniformLocation(shaderProgramSphere, "light_position");
	lightAmbientUniformLocation = glGetUniformLocation(shaderProgramSphere, "light_ambient");
	lightDiffuseUniformLocation = glGetUniformLocation(shaderProgramSphere, "light_diffuse");
	lightSpecularUniformLocation = glGetUniformLocation(shaderProgramSphere, "light_specular");

	shininessUniformLocation = glGetUniformLocation(shaderProgramSphere, "shininess");
	colorSpecularUniformLocation = glGetUniformLocation(shaderProgramSphere, "color_specular");
	colorEmittedUniformLocation = glGetUniformLocation(shaderProgramSphere, "color_emitted");


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