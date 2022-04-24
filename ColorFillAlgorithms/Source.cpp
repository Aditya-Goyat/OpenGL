#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <irrKlang.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <math.h>

#pragma comment(lib, "irrKlang.lib")

using namespace std;
using namespace irrklang;

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 inColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"gl_Position = model * vec4(position.x, position.y, position.z, 1.0);\n"
"ourColor = inColor;\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"color = vec4(ourColor, 1.0f);\n"
"}\n\0";

const GLchar* vertexShaderSource1 = "#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0f, 1.0f);\n"
"}\0";

const GLchar* fragmentShaderSource1 = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

const GLint WIDTH = 800, HEIGHT = 800;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void BoundaryFill(int x, int y, float fillColor[3], float borderColor[3]);
void FloodFill(int x, int y, float oldColor[3]);

bool okay = false;
vector<float> vert;

float color[3];
float xNDC, yNDC;

float oldColor[3] = { 1.0,1.0,1.0 };

int colored[WIDTH][HEIGHT];

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "500080575_AdityaGoyat", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glewInit();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	GLuint vertexShader1, fragmentShader1;
	vertexShader1 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader1, 1, &vertexShaderSource1, NULL);
	glCompileShader(vertexShader1);
	fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
	glCompileShader(fragmentShader1);
	GLuint shaderProgram1 = glCreateProgram(); 
	glAttachShader(shaderProgram1, vertexShader1);
	glAttachShader(shaderProgram1, fragmentShader1);
	glLinkProgram(shaderProgram1);

	float vertices[] = {
	 0.5f,  0.5f, 0.0f,  1.0, 1.0, 1.0,// top right
	 0.5f, -0.5f, 0.0f,  1.0, 1.0, 1.0,// bottom right
	-0.5f, -0.5f, 0.0f,  1.0, 1.0, 1.0,// bottom left
	-0.5f,  0.5f, 0.0f,  1.0, 1.0, 1.0// top left 
	};
	unsigned int indices[] = { 
		0, 1, 3,  
		1, 2, 3   
	};

	float verticesSquare[] = {
	 0.5f,  0.5f, 0.0f,  1.0, 0.0, 0.0,// top right
	 0.5f, -0.5f, 0.0f,  1.0, 0.0, 0.0,// bottom right
	-0.5f, -0.5f, 0.0f,  1.0, 0.0, 0.0,// bottom left
	-0.5f,  0.5f, 0.0f,  1.0, 0.0, 0.0// top left 
	};
	unsigned int indicesSquare[] = {
		0, 3,
		3, 2,
		2, 1,
		1, 0
	};

	GLuint VBO[2], VAO[2], EBO[2];

	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), verticesSquare, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesSquare), indicesSquare, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	int x = 200;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO[0]);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO[1]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		if (okay) {
			GLuint VAO1, VBO1;
			glGenVertexArrays(1, &VAO1);
			glGenBuffers(1, &VBO1);
			glBindVertexArray(VAO1);
			glBindBuffer(GL_ARRAY_BUFFER, VBO1);
			glBufferData(GL_ARRAY_BUFFER, (vert.size() * sizeof(float)), &vert[0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glUseProgram(shaderProgram1);
			glBindVertexArray(VAO1);

			glDrawArrays(GL_POINTS, 0, vert.size() / 2);
			glBindVertexArray(0);

			glDeleteVertexArrays(1, &VAO1);
			glDeleteBuffers(1, &VBO1);
		}
		glfwSwapBuffers(window);
	}

	glDeleteBuffers(2, VBO);
	glDeleteVertexArrays(2, VAO);
	glDeleteProgram(shaderProgram);
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		float fillCol[3] = { 1.0,0.0,0.0 };
		float borderCol[3] = { 1.0,0.0,0.0 };
		glfwGetCursorPos(window, &x, &y);
		y = abs(800 - y);
		cout << x << " " << y << endl;
		BoundaryFill(x, y, fillCol, borderCol);
		okay = true;
 	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		glfwGetCursorPos(window, &x, &y);
		y = abs(HEIGHT - y);
		float oldColor[3] = { 1.0,1.0,1.0 };
		FloodFill(x, y, oldColor);
		okay = true;
	}
}

void BoundaryFill(int x, int y, float fillColor[3], float borderColor[3]) {
	glReadPixels(x, y, 1.0, 1.0, GL_RGB, GL_FLOAT, color);
	cout << x << " " << y << endl;
	if ((color[0] != borderColor[0] || color[1] != borderColor[1] || color[2] != borderColor[2]) && (color[0] != fillColor[0] || color[1] != fillColor[1] || color[2] != fillColor[2]) && colored[x][y] == 0)
		{
			cout << 1 << endl;
			xNDC = ((float)x / (float)WIDTH) * 2 - 1;
			yNDC = ((float)y / (float)HEIGHT) * 2 - 1;
			vert.push_back(xNDC);
			vert.push_back(yNDC);
			colored[x][y] = 1;
			BoundaryFill((x + 1), y, fillColor, borderColor);
			BoundaryFill((x - 1), y, fillColor, borderColor);
			BoundaryFill(x, (y + 1), fillColor, borderColor);
			BoundaryFill(x, (y - 1), fillColor, borderColor);
		}
}

void FloodFill(int x, int y, float oldColor[3]) {
	float color[3];
	glReadPixels(x, y, 1.0, 1.0, GL_RGB, GL_FLOAT, color);
	cout << x << " " << y << endl;
	if (color[0] == oldColor[0] && color[1] == oldColor[1] && color[2] == oldColor[2] && (colored[x][y] == 0))
	{
		cout << 1 << endl;
		xNDC = ((float)x / (float)WIDTH) * 2 - 1;
		yNDC = ((float)y / (float)HEIGHT) * 2 - 1;
		vert.push_back(xNDC);
		vert.push_back(yNDC);
		colored[x][y] = 1;
		FloodFill((x + 1), y, oldColor);  
		FloodFill(x, (y + 1), oldColor);
		FloodFill((x - 1), y, oldColor);
		FloodFill(x, (y - 1), oldColor);
		FloodFill((x + 1), y+1, oldColor);
		FloodFill((x - 1), y-1, oldColor);
		FloodFill(x-1, (y + 1), oldColor);
		FloodFill(x+1, (y - 1), oldColor);
	}
	return;
}