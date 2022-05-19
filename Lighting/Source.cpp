#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <irrKlang.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <glm/gtc/type_ptr.hpp> 

#pragma comment(lib, "irrKlang.lib")

using namespace std;
using namespace irrklang;

//Gourard Shading:
//const GLchar* vertexShaderSource = "#version 330 core\n"
//"layout (location = 0) in vec3 position;\n"
//"layout (location = 1) in vec3 aNormal;\n"
//"uniform mat4 model;\n"
//"uniform mat4 view;\n"
//"uniform mat4 projection;\n"
//"uniform vec3 lightPos;\n"
//"uniform vec3 lightColor;\n"
//"uniform vec3 viewPos;\n"
//"out vec3 LightingColor;\n"
//"void main()\n"
//"{\n"
//"gl_Position = projection * view * model * vec4(position, 1.0);\n"
//"vec3 Position = vec3(model * vec4(position, 1.0));\n"
//"vec3 Normal = mat3(transpose(inverse(model))) * aNormal;\n"
//"float ambientStrength = 0.1;\n"
//"vec3 ambient = ambientStrength * lightColor;\n"
//"vec3 norm = normalize(Normal);\n"
//"vec3 lightDir = normalize(lightPos - Position);\n"
//"float diff = max(dot(norm, lightDir), 0.0);\n"
//"vec3 diffuse = diff * lightColor;\n"
//"float specularStrength = 1.0;\n"
//"vec3 viewDir = normalize(viewPos - Position);\n"
//"vec3 reflectDir = reflect(-lightDir, norm);\n"
//"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
//"vec3 specular = specularStrength * spec * lightColor;\n"
//"LightingColor = ambient + diffuse + specular;\n"
//"}\0";
//
//const GLchar* fragmentShaderSource = "#version 330 core\n"
//"in vec3 LightingColor;\n"
//"out vec4 color;\n"
//"uniform vec3 objectColor;\n"
//"void main()\n"
//"{\n"
//"color = vec4(LightingColor * objectColor, 1.0);\n"
//"}\n\0";

//Phong Shading
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 aNormal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 normal;\n"
"out vec3 fragPosition;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(position, 1.0);\n"
"normal = aNormal;\n"
"fragPosition = vec3(model*vec4(position,1.0));\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"in vec3 normal;\n"
"in vec3 fragPosition;\n"
"out vec4 color;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 viewPos;\n"
"void main()\n"
"{\n"
"float ambientStrength = 0.15;\n"
"vec3 ambient = ambientStrength * lightColor;\n"
"vec3 norm = normalize(normal);\n"
"vec3 lightDir = normalize(lightPos - fragPosition);\n"
"float diff = max(dot(norm, lightDir), 0.0);\n"
"vec3 diffuse = diff * lightColor;\n"
"float specularStrength = 0.5;\n"
"vec3 viewDir = normalize(viewPos - fragPosition);\n"
"vec3 reflectDir = reflect(-lightDir, norm);\n"
"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);\n"
"vec3 specular = specularStrength * spec * lightColor;\n"
"vec3 result = (ambient + specular + diffuse) * objectColor;\n"
"color = vec4(result, 1.0f);\n"
"}\n\0";

const GLchar* vertexShaderSourceLight = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(position, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSourceLight = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\n\0";

const int WIDTH = 800;
const int HEIGHT = 800;

void CreateShaders(GLuint* fragmentShader, GLuint* vertexShader, const GLchar* fragmentShaderSource, const GLchar* vertexShaderSource);
void CreateProgram(GLuint* fragmentShader, GLuint* vertexShader, GLuint* shaderProgram);
void CreateBuffersLight(GLuint* VBO, GLuint* VAO, GLfloat vertices[], GLint size);
void CreateBuffersModel(GLuint* VBO, GLuint* VAO, GLfloat vertices[], GLint size);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 cameraPos;
float radius = 4.0f;

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Phong Shading", NULL, NULL);

	glfwMakeContextCurrent(window);

	glewInit();

	GLuint fragmentShader, vertexShader;
	CreateShaders(&fragmentShader, &vertexShader, fragmentShaderSource, vertexShaderSource);

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
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShaderLight, vertexShaderLight;
	CreateShaders(&fragmentShaderLight, &vertexShaderLight, fragmentShaderSourceLight, vertexShaderSourceLight);

	GLuint shaderProgramLight;
	CreateProgram(&fragmentShaderLight, &vertexShaderLight, &shaderProgramLight);

	GLuint shaderProgram;
	CreateProgram(&fragmentShader, &vertexShader, &shaderProgram);

	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	GLuint VBOs[2], VAOs[2];
	CreateBuffersModel(&VBOs[0], &VAOs[0], vertices, sizeof(vertices));
	CreateBuffersLight(&VBOs[1], &VAOs[1], vertices, sizeof(vertices));

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
		GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		glUniform3fv(lightPosLoc, 1, &lightPos[0]);

		float cameraX = sin(glfwGetTime()) * radius;
		float cameraY = cos(glfwGetTime()) * radius;
		cameraPos = glm::vec3(cameraX, 1.0, cameraY);

		GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3fv(viewPosLoc, 1, &cameraPos[0]);

		glm::mat4 projection = glm::perspective(glm::radians(65.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 model(1.0f);

		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUseProgram(shaderProgramLight);

		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		modelLoc = glGetUniformLocation(shaderProgramLight, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		viewLoc = glGetUniformLocation(shaderProgramLight, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		projectionLoc = glGetUniformLocation(shaderProgramLight, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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

void CreateBuffersLight(GLuint* VBO, GLuint* VAO, GLfloat vertices[], GLint size) {
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void CreateBuffersModel(GLuint* VBO, GLuint* VAO, GLfloat vertices[], GLint size) {
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);

	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}