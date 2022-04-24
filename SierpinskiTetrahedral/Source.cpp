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
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;
using namespace irrklang;

#pragma comment(lib, "irrKlang.lib")

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 inColor;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"out vec3 Color;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(position, 1.0);\n"
"Color = inColor;\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec3 Color;\n"
"void main()\n"
"{\n"
"color = vec4(Color, 1.0f);\n"
"}\n\0";

const GLchar* vertexShaderTextureSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout(location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0);\n"
"TexCoord = aTexCoord;\n"
"}\0";

const GLchar* fragmentShaderTextureSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"color = texture(ourTexture, TexCoord);\n"
"}\n\0";

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void processInput(GLFWwindow* window);
void DrawTetra(float V1[], float V2[], float V3[], float V4[], vector<float>& vertices);
void DrawTriangle(float A[], float B[], float C[], vector<float>& vertices, string color);
void Div(float V1[], float V2[], float V3[], float V4[], vector<float>& vertices, int n);

const GLint WIDTH = 800, HEIGHT = 800;

float scale = 1.0f;
int n = -1;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float P[4][3] = {
	{-0.5f, -0.5f, 0.5f},
	{0.5f, -0.5f, 0.5f},
	{0.0f, 0.5f, 0.5f},
	{0.0f, 0.0f, -0.5f}
};

vector<float> vertices;

bool okay = false;

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Sierpinsky Tetrahedral", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewInit();

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GLuint vertexShader, fragmentShader, shaderProgram;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint vertexShaderTexture, fragmentShaderTexture, shaderProgramTexture;

	vertexShaderTexture = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderTexture, 1, &vertexShaderTextureSource, NULL);
	glCompileShader(vertexShaderTexture);
	fragmentShaderTexture = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderTexture, 1, &fragmentShaderTextureSource, NULL);
	glCompileShader(fragmentShaderTexture);
	shaderProgramTexture = glCreateProgram();
	glAttachShader(shaderProgramTexture, vertexShaderTexture);
	glAttachShader(shaderProgramTexture, fragmentShaderTexture);
	glLinkProgram(shaderProgramTexture);

	glDeleteShader(vertexShaderTexture);
	glDeleteShader(fragmentShaderTexture);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float verticesTexture[] = {
		// positions          // texture coords
		-0.5f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
		-0.5f, 0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
		-1.0f, 0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
		-1.0f, 1.0, 0.0f,    0.0f, 1.0f    // top left 
	};

	unsigned int indicesTexture[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBOTexutre, VAOTexture, EBOTexture;
	glGenVertexArrays(1, &VAOTexture);
	glGenBuffers(1, &VBOTexutre);
	glGenBuffers(1, &EBOTexture);

	glBindVertexArray(VAOTexture);

	glBindBuffer(GL_ARRAY_BUFFER, VBOTexutre);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTexture), verticesTexture, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOTexture);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesTexture), indicesTexture, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);

	GLuint texture;
	int width, height;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* image = SOIL_load_image("Name_SAP_Flip.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	while (!glfwWindowShouldClose(window)) {

		processInput(window);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.67f, 0.67f, 0.67f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgramTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shaderProgramTexture, "ourTexture"), 0);
		glBindVertexArray(VAOTexture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glUseProgram(shaderProgram);

		glm::mat4 model(1.0f);
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		if (okay) {
			GLuint VBO, VAO;

			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(float)), &vertices[0], GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);

			glDeleteBuffers(1, &VBO);
			glDeleteVertexArrays(1, &VAO);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);
	glfwTerminate();

	return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(1.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void DrawTetra(float V1[], float V2[], float V3[], float V4[], vector<float>& vertices) {

	string s[4] = { "red", "green", "blue", "yellow" };
	random_shuffle(&s[0], &s[3]);
	//DrawTriangle(V1, V2, V3, vertices, s[0]);
	//DrawTriangle(V1, V3, V4, vertices, s[1]);
	//DrawTriangle(V2, V3, V4, vertices, s[2]);
	//DrawTriangle(V1, V2, V4, vertices, s[3]);

	DrawTriangle(V1, V2, V3, vertices, "white");
	DrawTriangle(V1, V3, V4, vertices, "black");
	DrawTriangle(V2, V3, V4, vertices, "off-white");
	DrawTriangle(V1, V2, V4, vertices, "off-black");
}

void DrawTriangle(float A[], float B[], float C[], vector<float>& vertices, string color) {
	vertices.push_back(A[0]);
	vertices.push_back(A[1]);
	vertices.push_back(A[2]);
	if (color == "red") {
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "green") {
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "blue") {
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
	}
	else if (color == "black") {
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "white") {
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
	}
	else if (color == "off-black") {
		vertices.push_back(0.64);
		vertices.push_back(0.64);
		vertices.push_back(0.64);
	}
	else if (color == "off-white") {
		vertices.push_back(0.85f);
		vertices.push_back(0.87f);
		vertices.push_back(0.86f);
	}
	else {
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
	}

	vertices.push_back(B[0]);
	vertices.push_back(B[1]);
	vertices.push_back(B[2]);
	if (color == "red") {
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "green") {
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "blue") {
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
	}
	else if (color == "black") {
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "white") {
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
	}
	else if (color == "off-black") {
		vertices.push_back(0.64);
		vertices.push_back(0.64);
		vertices.push_back(0.64);
	}
	else if (color == "off-white") {
		vertices.push_back(0.85f);
		vertices.push_back(0.87f);
		vertices.push_back(0.86f);
	}
	else {
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
	}

	vertices.push_back(C[0]);
	vertices.push_back(C[1]);
	vertices.push_back(C[2]);
	if (color == "red") {
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "green") {
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "blue") {
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(1.0f);
	}
	else if (color == "black") {
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
		vertices.push_back(0.0f);
	}
	else if (color == "white") {
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
	}
	else if (color == "off-black") {
		vertices.push_back(0.64);
		vertices.push_back(0.64);
		vertices.push_back(0.64);
	}
	else if (color == "off-white") {
		vertices.push_back(0.85f);
		vertices.push_back(0.87f);
		vertices.push_back(0.86f);
	}
	else {
		vertices.push_back(1.0f);
		vertices.push_back(1.0f);
		vertices.push_back(0.0f);
	}
}

void Div(float V1[], float V2[], float V3[], float V4[], vector<float>& vertices, int n) {
	float V12[3], V23[3], V31[3], V14[3], V24[3], V34[3];
	cout << "DIV" << endl;
	if (n > 0) {
		V12[0] = (V1[0] + V2[0]) / 2;
		V12[1] = (V1[1] + V2[1]) / 2;
		V12[2] = (V1[2] + V2[2]) / 2;

		V23[0] = (V3[0] + V2[0]) / 2;
		V23[1] = (V3[1] + V2[1]) / 2;
		V23[2] = (V3[2] + V2[2]) / 2;

		V31[0] = (V3[0] + V1[0]) / 2;
		V31[1] = (V3[1] + V1[1]) / 2;
		V31[2] = (V3[2] + V1[2]) / 2;

		V14[0] = (V1[0] + V4[0]) / 2;
		V14[1] = (V1[1] + V4[1]) / 2;
		V14[2] = (V1[2] + V4[2]) / 2;

		V24[0] = (V4[0] + V2[0]) / 2;
		V24[1] = (V4[1] + V2[1]) / 2;
		V24[2] = (V4[2] + V2[2]) / 2;

		V34[0] = (V3[0] + V4[0]) / 2;
		V34[1] = (V3[1] + V4[1]) / 2;
		V34[2] = (V3[2] + V4[2]) / 2;

		Div(V1, V12, V31, V14, vertices, n - 1);
		Div(V12, V2, V23, V24, vertices, n - 1);
		Div(V31, V23, V3, V34, vertices, n - 1);
		Div(V14, V24, V34, V4, vertices, n - 1);

	}
	else
		DrawTetra(V1, V2, V3, V4, vertices);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		okay = true;
		n++;
		vertices.clear();
		Div(P[0], P[1], P[2], P[3], vertices, n);
	}
}