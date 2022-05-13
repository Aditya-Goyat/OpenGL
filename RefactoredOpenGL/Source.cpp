#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <irrKlang.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#pragma comment(lib, "irrKlang.lib")

using namespace std;
using namespace irrklang;

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

const int WIDTH = 800;
const int HEIGHT = 800;

void CreateShaders(GLuint* fragmentShader, GLuint* vertexShader, const GLchar* fragmentShaderSource, const GLchar* vertexShaderSource);
void CreateProgram(GLuint* fragmentShader, GLuint* vertexShader, GLuint* shaderProgram);
void CreateBuffers(GLuint* VBO, GLuint* VAO, GLfloat vertices[], GLint size);

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Refactor", NULL, NULL);

	glfwMakeContextCurrent(window);

	glewInit();

	GLuint fragmentShader, vertexShader;
	CreateShaders(&fragmentShader, &vertexShader, fragmentShaderSource, vertexShaderSource);

	GLuint shaderProgram;
	CreateProgram(&fragmentShader, &vertexShader, &shaderProgram);

	GLfloat verticesT1[] = {
		-0.9f, -0.5f, 0.0f,  // Left 
		-0.0f, -0.5f, 0.0f,  // Right
		-0.45f, 0.5f, 0.0f,  // Top 
	};

	GLfloat verticesT2[] = {
		0.0f, -0.5f, 0.0f,  // Left
		0.9f, -0.5f, 0.0f,  // Right
		0.45f, 0.5f, 0.0f   // Top 
	};
	//Even though the above two traingle vertices could be put together into one, I gave the vertex values separately in order to demonstrate the use of the CreateBuffers function to easily create multiple buffers.

	GLuint VBOs[2], VAOs[2];
	CreateBuffers(&VBOs[0], &VAOs[0], verticesT1, sizeof(verticesT1));
	CreateBuffers(&VBOs[1], &VAOs[1], verticesT2, sizeof(verticesT2));

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}

	glDeleteBuffers(2, VBOs);
	glDeleteVertexArrays(2, VAOs);
	glDeleteProgram(shaderProgram);
	glfwTerminate();

	return 0;
}

void CreateShaders(GLuint* fragmentShader, GLuint* vertexShader, const GLchar* fragmentShaderSource, const GLchar* vertexShaderSource) {
	*vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(*vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(*vertexShader);

	*fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(*fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(*fragmentShader);
}

void CreateProgram(GLuint* fragmentShader, GLuint* vertexShader, GLuint* shaderProgram) {
	*shaderProgram = glCreateProgram();
	glAttachShader(*shaderProgram, *vertexShader);
	glAttachShader(*shaderProgram, *fragmentShader);
	glLinkProgram(*shaderProgram);

	glDeleteShader(*fragmentShader);
	glDeleteShader(*vertexShader);
}

void CreateBuffers(GLuint* VBO, GLuint* VAO, GLfloat vertices[], GLint size) {
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}