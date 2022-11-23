#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <irrKlang.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include "Shader.h"
#include <vector>
#include <math.h>

#pragma comment(lib, "irrKlang.lib")

using namespace std;
using namespace irrklang;

const int WIDTH = 1000, HEIGHT = 1000;
const float TARGET_FPS = 60;

void pushVertex(int x, int y, vector<float> &vertices);

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lab02", NULL, NULL);
	if (window == NULL) {
		cout << "failed to create window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		cout << "glew not initialized" << endl;
	}

	Shader shaders("shader.vs", "shader.fs");

	vector<float> vertices;

	pushVertex(571, 429, vertices);
	pushVertex(500, 400, vertices);
	pushVertex(500, 400, vertices);
	pushVertex(429, 429, vertices);
	pushVertex(429, 429, vertices);
	pushVertex(400, 500, vertices);
	pushVertex(400, 500, vertices);
	pushVertex(429, 571, vertices);
	pushVertex(429, 571, vertices);
	pushVertex(500, 600, vertices);
	pushVertex(500, 600, vertices);
	pushVertex(571, 571, vertices);
	pushVertex(571, 571, vertices);
	pushVertex(600, 500, vertices);
	pushVertex(600, 500, vertices);
	pushVertex(571, 429, vertices);

	pushVertex(641, 359, vertices);
	pushVertex(500, 300, vertices);
	pushVertex(500, 300, vertices);
	pushVertex(359, 359, vertices);
	pushVertex(359, 359, vertices);
	pushVertex(300, 500, vertices);
	pushVertex(300, 500, vertices);
	pushVertex(359, 641, vertices);
	pushVertex(359, 641, vertices);
	pushVertex(500, 700, vertices);
	pushVertex(500, 700, vertices);
	pushVertex(641, 641, vertices);
	pushVertex(641, 641, vertices);
	pushVertex(700, 500, vertices);
	pushVertex(700, 500, vertices);
	pushVertex(641, 359, vertices);

	pushVertex(712, 288, vertices);
	pushVertex(500, 200, vertices);
	pushVertex(500, 200, vertices);
	pushVertex(288, 288, vertices);
	pushVertex(288, 288, vertices);
	pushVertex(200, 500, vertices);
	pushVertex(200, 500, vertices);
	pushVertex(288, 712, vertices);
	pushVertex(288, 712, vertices);
	pushVertex(500, 800, vertices);
	pushVertex(500, 800, vertices);
	pushVertex(712, 712, vertices);
	pushVertex(712, 712, vertices);
	pushVertex(800, 500, vertices);
	pushVertex(800, 500, vertices);
	pushVertex(712, 288, vertices);

	pushVertex(0, 500, vertices);
	pushVertex(1000, 500, vertices);
	pushVertex(500, 1000, vertices);
	pushVertex(500, 0, vertices);
	pushVertex(0, 0, vertices);
	pushVertex(1000, 1000, vertices);
	pushVertex(0, 1000, vertices);
	pushVertex(1000, 0, vertices);

	GLfloat vertice[] = {
		-1.0f, -1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f
	};

	GLuint VBO, VAO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(float)), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	double lasttime = glfwGetTime();

	float a = 2;
	int n = floor(a);
	//Game Loop
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaders.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, n);

		glfwPollEvents();
		glfwSwapBuffers(window);

		a += (1.0 / TARGET_FPS) * 16;
		if (a >= (vertices.size() / 2))
			a = (vertices.size() / 2);

		n = floor(a);

		while (glfwGetTime() < lasttime + 1.0 / TARGET_FPS) {
			// TODO: Put the thread to sleep, yield, or simply do nothing
		}
		lasttime += 1.0 / TARGET_FPS;
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void pushVertex(int x, int y, vector<float>& vertices) {
	float xNDC, yNDC;

	xNDC = ((float)x / (float)WIDTH) * 2 - 1;
	yNDC = ((float)y / (float)HEIGHT) * 2 - 1;
	vertices.push_back(xNDC);
	vertices.push_back(yNDC);
}