#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <irrKlang.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <math.h>
#include <vector>
#include <time.h>
#include <algorithm>

#pragma comment(lib, "irrKlang.lib")

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 inColor;\n"
"uniform mat4 transformation;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"out vec3 Color;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * transformation * vec4(position, 1.0);\n"
"Color = inColor;\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec3 Color;\n"
"void main()\n"
"{\n"
"color = vec4(Color, 1.0f);\n"
"}\n\0";

const GLchar* vertexShaderSourceAxes = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * vec4(position, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSourceAxes = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
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

using namespace std;
using namespace irrklang;

float dx = 400, dy = 400, dz = 400, thetax = 0, thetay = 0, thetaz = 0, sx = 1, sy = 1, sz = 1, rx = 1, ry = 1, rz = 1;
float dxNDC = 0, dyNDC = 0, dzNDC = 0;
float speed = 10, rotSpeed = 0.1f;
int n = -1;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true, menu = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void DrawTetra(float V1[], float V2[], float V3[], float V4[], vector<float>& vertices);
void DrawTriangle(float A[], float B[], float C[], vector<float>& vertices, string color);
void Div(float V1[], float V2[], float V3[], float V4[], vector<float>& vertices, int n);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);


glm::mat4 translate(1.0f);
glm::mat4 scale = glm::mat4(1.0f);
glm::mat4 rotatex = glm::mat4(1.0f);
glm::mat4 rotatey = glm::mat4(1.0f);
glm::mat4 rotatez = glm::mat4(1.0f);
glm::mat4 reflection(1.0f);
glm::mat4 model(1.0f);

bool rot = false, trans = false, sc = false, refl = false;

const GLint WIDTH = 800, HEIGHT = 800;

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

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SierpinskyTetrahedral", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewInit();

	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

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

	GLuint vertexShaderAxes, fragmentShaderAxes, shaderProgramAxes;

	vertexShaderAxes = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderAxes, 1, &vertexShaderSourceAxes, NULL);
	glCompileShader(vertexShaderAxes);

	fragmentShaderAxes = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderAxes, 1, &fragmentShaderSourceAxes, NULL);
	glCompileShader(fragmentShaderAxes);

	glGetShaderiv(vertexShaderAxes, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShaderAxes, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShaderAxes, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShaderAxes, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	shaderProgramAxes = glCreateProgram();
	glAttachShader(shaderProgramAxes, vertexShaderAxes);
	glAttachShader(shaderProgramAxes, fragmentShaderAxes);
	glLinkProgram(shaderProgramAxes);

	glDeleteShader(vertexShaderAxes);
	glDeleteShader(fragmentShaderAxes);

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

	float verticesTexture1[] = {
		// positions          // texture coords
		0.25f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
		0.25f, 0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
		1.0f, 0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
		1.0f, 1.0, 0.0f,    0.0f, 1.0f    // top left 
	};

	unsigned int indicesTexture1[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int VBOTexutre1, VAOTexture1, EBOTexture1;
	glGenVertexArrays(1, &VAOTexture1);
	glGenBuffers(1, &VBOTexutre1);
	glGenBuffers(1, &EBOTexture1);

	glBindVertexArray(VAOTexture1);

	glBindBuffer(GL_ARRAY_BUFFER, VBOTexutre1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTexture1), verticesTexture1, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOTexture1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesTexture1), indicesTexture1, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	float verticesAxes[] = {
		0.0f, 0.0f, -100.0f,
		0.0f, 0.0f, 10.0f,
		0.0f, -10.0f, 0.0f,
		0.0f, 10.0f, 0.0f,
		10.0f, 0.0f, 0.0f,
		-10.0f, 0.0f, 0.0f
	};

	GLuint VBOAxes, VAOAxes;

	glGenVertexArrays(1, &VAOAxes);
	glGenBuffers(1, &VBOAxes);

	glBindVertexArray(VAOAxes);

	glBindBuffer(GL_ARRAY_BUFFER, VBOAxes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAxes), &verticesAxes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	GLuint texture1;
	int width1, height1;

	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* image1 = SOIL_load_image("Menu_Flip3.png", &width1, &height1, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1);

	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image1);
	glBindTexture(GL_TEXTURE_2D, 0);

	while (!glfwWindowShouldClose(window)) {

		processInput(window);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.18f, 0.30f, 0.30f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgramTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shaderProgramTexture, "ourTexture"), 0);
		glBindVertexArray(VAOTexture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		if (menu == true) {
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(shaderProgramTexture, "ourTexture"), 0);
			glBindVertexArray(VAOTexture1);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		translate[3] = glm::vec4(glm::vec3(dxNDC, dyNDC, dzNDC), 1);

		rotatex[1].y = cos(thetax);
		rotatex[1].z = -sin(thetax);
		rotatex[2].y = sin(thetax);
		rotatex[2].z = cos(thetax);

		rotatey[0].x = cos(thetay);
		rotatey[0].z = sin(thetay);
		rotatey[2].x = -sin(thetay);
		rotatey[2].z = cos(thetay);

		rotatez[0].x = cos(thetaz);
		rotatez[0].y = -sin(thetaz);
		rotatez[1].x = sin(thetaz);
		rotatez[1].y = cos(thetaz);

		scale[0].x = sx;
		scale[1].y = sy;
		scale[2].z = sz;

		reflection[0].x = rx;
		reflection[1].y = ry;
		reflection[2].z = rz;

		model = reflection * translate * rotatez * rotatey * rotatex * scale;

		glUseProgram(shaderProgram);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		GLuint modelLoc = glGetUniformLocation(shaderProgram, "transformation");
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

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

		glUseProgram(shaderProgramAxes);
		viewLoc = glGetUniformLocation(shaderProgramAxes, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		projectionLoc = glGetUniformLocation(shaderProgramAxes, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAOAxes);
		glDrawArrays(GL_LINES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shaderProgram);
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		trans = true;
		rot = false;
		sc = false;
		refl = false;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		rot = true;
		trans = false;
		sc = false;
		refl = false;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		sc = true;
		trans = false;
		rot = false;
		refl = false;
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		sc = false;
		trans = false;
		rot = false;
		refl = true;
	}
	if (key == GLFW_KEY_R and action == GLFW_PRESS) {
		dx = 400;
		dy = 400;
		dz = 400;
		dxNDC = 0;
		dyNDC = 0;
		dzNDC = 0;
		thetax = 0;
		thetay = 0;
		thetaz = 0;
		sx = 1;
		sy = 1;
		sz = 1;
		rx = 1;
		ry = 1;
		rz = 1;
	}
	if (key == GLFW_KEY_H and action == GLFW_PRESS)
		menu = !menu;

	if (trans) {
		if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			dx += speed;
			dxNDC = (dx / (float)WIDTH) * 2 - 1;
		}
		if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			dz -= speed;
			dzNDC = (dz / (float)WIDTH) * 2 - 1;
		}
		if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			dx -= speed;
			dxNDC = (dx / (float)WIDTH) * 2 - 1;
		}
		if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			dz += speed;
			dzNDC = (dz / (float)WIDTH) * 2 - 1;
		}
		if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			dy += speed;
			dyNDC = (dy / (float)WIDTH) * 2 - 1;
		}
		if (key == GLFW_KEY_TAB && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			dy -= speed;
			dyNDC = (dy / (float)WIDTH) * 2 - 1;
		}
	}

	if (sc) {
		if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			if (sz > 1)
				sz += float(speed / 10);
			else if (sz <= 1 && sz >= 0)
				sz += 0.05f;
		}
		if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			if (sz > 1)
				sz -= float(speed / 10);
			else if (sz <= 1 && sz >= 0)
				sz -= 0.05f;

			if (sz <= 0.05)
				sz = 0.05;
		}
		if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			if (sx > 1)
				sx -= float(speed / 10);
			else if (sx <= 1 && sx >= 0)
				sx -= 0.05f;

			if (sx <= 0.05)
				sx = 0.05;
		}
		if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			if (sx > 1)
				sx += float(speed / 10);
			else if (sx <= 1 && sx >= 0)
				sx += 0.05f;
		}
		if (key == GLFW_KEY_TAB && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			if (sy > 1)
				sy -= float(speed / 10);
			else if (sy <= 1 && sy >= 0)
				sy -= 0.05f;

			if (sy <= 0.05)
				sy = 0.05;
		}
		if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			if (sy > 1)
				sy += float(speed / 10);
			else if (sy <= 1 && sy >= 0)
				sy += 0.05f;
		}
	}

	if (rot) {
		if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			thetax+= rotSpeed;
		}
		if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			thetax-= rotSpeed;
		}
		if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			thetaz-= rotSpeed;
		}
		if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			thetaz+= rotSpeed;
		}
		if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			thetay+= rotSpeed;
		}
		if (key == GLFW_KEY_TAB && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			thetay-= rotSpeed;
		}
	}

	if (refl) {
		if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			rz *= -1;
		}
		if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			rx *= -1;
		}
		if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
			ry *= -1;
		}
	}

	if (key == GLFW_KEY_N &&(action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		okay = true;
		n++;
		vertices.clear();
		Div(P[0], P[1], P[2], P[3], vertices, n);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
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

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = static_cast<float>(1.5 * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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