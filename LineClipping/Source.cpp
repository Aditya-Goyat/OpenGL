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
"layout(location = 1) in vec3 inColor;\n"
"out vec3 Color;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0f);\n"
"Color = inColor;\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec3 Color;\n"
"void main()\n"
"{\n"
"color = vec4(Color, 1.0f);\n"
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

const GLchar* fragmentShaderSource2 = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
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

const GLint WIDTH = 800, HEIGHT = 800;
GLint xmax = 300, xmin = 100, ymax = 300, ymin = 100;
GLint xmax1 = 600, xmin1 = 400, ymax1 = 600, ymin1 = 400;
int a0 = 400, b0 = 800, a1 = 400, b1 = 0;
float e, f, g, h;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Cohen(int x0, int y0, int x1, int y1);
void Liang(int x1, int y1, int x2, int y2);
int ComputeCode(double x, double y);

bool okay = false, okay1 = false;
vector<float> vert;

float vertex[4], vertex1[4];

float xNDC, yNDC, xMinNDC, xMaxNDC, yMinNDC, yMaxNDC;
glm::mat4 model = glm::mat4(1.0f);

const int RIGHT = 8;
int LEFT =2;
int TOP=4;
int BOTTOM=1; 

int speed = 5;
double p[4], q[4];
double u1 = 0, u2 = 1;

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint vertexShader, fragmentShader, shaderProgram;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLuint vertexShader1, fragmentShader1, shaderProgram1;
	vertexShader1 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader1, 1, &vertexShaderSource1, NULL);
	glCompileShader(vertexShader1);

	fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
	glCompileShader(fragmentShader1);

	shaderProgram1 = glCreateProgram();
	glAttachShader(shaderProgram1, vertexShader1);
	glAttachShader(shaderProgram1, fragmentShader1);
	glLinkProgram(shaderProgram1);

	glDeleteShader(vertexShader1);
	glDeleteShader(fragmentShader1);

	GLuint fragmentShader2, shaderProgram2;
	fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
	glCompileShader(fragmentShader2);

	shaderProgram2 = glCreateProgram();
	glAttachShader(shaderProgram2, vertexShader1);
	glAttachShader(shaderProgram2, fragmentShader2);
	glLinkProgram(shaderProgram2);

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
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgramTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(shaderProgramTexture, "ourTexture"), 0);
		glBindVertexArray(VAOTexture);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		xMinNDC = ((float)xmin / (float)WIDTH) * 2 - 1;
		yMinNDC = ((float)ymin / (float)HEIGHT) * 2 - 1;
		xMaxNDC = ((float)xmax / (float)WIDTH) * 2 - 1;
		yMaxNDC = ((float)ymax / (float)HEIGHT) * 2 - 1;
		glUseProgram(shaderProgram);

 		float vertices[] = {
		 xMaxNDC,  yMaxNDC, 0.0f,  0.0f, 0.0f, 0.0f,// top right
		 xMaxNDC,  yMinNDC, 0.0f,  0.0f, 0.0f, 0.0f,// bottom right
		 xMinNDC,  yMinNDC, 0.0f,  0.0f, 0.0f, 0.0f,// bottom left
		 xMinNDC,  yMaxNDC, 0.0f,  0.0f, 0.0f, 0.0f// top left 
		};
		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		GLuint VBO, VAO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);


		xMinNDC = ((float)xmin1 / (float)WIDTH) * 2 - 1;
		yMinNDC = ((float)ymin1 / (float)HEIGHT) * 2 - 1;
		xMaxNDC = ((float)xmax1 / (float)WIDTH) * 2 - 1;
		yMaxNDC = ((float)ymax1 / (float)HEIGHT) * 2 - 1;
		float vertices1[] = {
		 xMaxNDC,  yMaxNDC, 0.0f,  0.0f, 0.0f, 0.0f,// top right
		 xMaxNDC,  yMinNDC, 0.0f,  0.0f, 0.0f, 0.0f,// bottom right
		 xMinNDC,  yMinNDC, 0.0f,  0.0f, 0.0f, 0.0f,// bottom left
		 xMinNDC,  yMaxNDC, 0.0f,  0.0f, 0.0f, 0.0f// top left 
		};
		unsigned int indices1[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		GLuint VBO3, VAO3, EBO3;
		glGenVertexArrays(1, &VAO3);
		glGenBuffers(1, &VBO3);
		glGenBuffers(1, &EBO3);
		glBindVertexArray(VAO3);
		glBindBuffer(GL_ARRAY_BUFFER, VBO3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO3);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glBindVertexArray(VAO3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glDeleteBuffers(1, &VBO3);
		glDeleteVertexArrays(1, &VAO3);

		xMinNDC = ((float)a0 / (float)WIDTH) * 2 - 1;
		yMinNDC = ((float)b0 / (float)HEIGHT) * 2 - 1;
		xMaxNDC = ((float)a1 / (float)WIDTH) * 2 - 1;
		yMaxNDC = ((float)b1 / (float)HEIGHT) * 2 - 1;
		float vertices2[] = {
		 xMinNDC,  yMinNDC,
		 xMaxNDC,  yMaxNDC
		};

		GLuint VBO2, VAO2;

		glGenVertexArrays(1, &VAO2);
		glGenBuffers(1, &VBO2);

		glBindVertexArray(VAO2);

		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glUseProgram(shaderProgram1);
		glBindVertexArray(VAO2);
		glDrawArrays(GL_LINES, 0, 2);

		if (okay) {
			GLuint VAO1, VBO1;
			glGenVertexArrays(1, &VAO1);
			glGenBuffers(1, &VBO1);
			glBindVertexArray(VAO1);
			glBindBuffer(GL_ARRAY_BUFFER, VBO1);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glUseProgram(shaderProgram2);
			glBindVertexArray(VAO1);

			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);

			glDeleteVertexArrays(1, &VAO1);
			glDeleteBuffers(1, &VBO1);
		}

		if (okay1) {
			GLuint VAO4, VBO4;
			glGenVertexArrays(1, &VAO4);
			glGenBuffers(1, &VBO4);
			glBindVertexArray(VAO4);
			glBindBuffer(GL_ARRAY_BUFFER, VBO4);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex1), vertex1, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glUseProgram(shaderProgram2);
			glBindVertexArray(VAO4);

			glDrawArrays(GL_LINES, 0, 2);
			glBindVertexArray(0);

			glDeleteVertexArrays(1, &VAO4);
			glDeleteBuffers(1, &VBO4);
		}
		glfwSwapBuffers(window);
	}

	glDeleteProgram(shaderProgram);
	glDeleteProgram(shaderProgram1);
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		ymax += speed;
		ymin += speed;
		Cohen(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		ymax -= speed;
		ymin -= speed;
		Cohen(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		xmax += speed;
		xmin += speed;
		Cohen(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		xmax -= speed;
		xmin -= speed;
		Cohen(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_W && action == GLFW_REPEAT) {
		ymax+= speed;
		ymin+= speed;
		Cohen(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_S && action == GLFW_REPEAT) {
		ymax -= speed;
		ymin -= speed;
		Cohen(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_D && action == GLFW_REPEAT) {
		xmax += speed;
		xmin += speed;
		Cohen(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_A && action == GLFW_REPEAT) {
		xmax -= speed;
		xmin -= speed;
		Cohen(a0, b0, a1, b1);
	}


	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		ymax1 += speed;
		ymin1 += speed;
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		ymax1 -= speed;
		ymin1 -= speed;
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		xmax1 += speed;
		xmin1 += speed;
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		xmax1 -= speed;
		xmin1 -= speed;
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_UP && action == GLFW_REPEAT) {
		ymax1 += speed;
		ymin1 += speed;
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_REPEAT) {
		ymax1 -= speed;
		ymin1 -= speed;
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_REPEAT) {
		xmax1 += speed;
		xmin1 += speed;
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_REPEAT) {
		xmax1 -= speed;
		xmin1 -= speed;
		Liang(a0, b0, a1, b1);
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		a0 += speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		a0 -= speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		b0 += speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		b0 -= speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_1 && action == GLFW_REPEAT) {
		a0 += speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_2 && action == GLFW_REPEAT) {
		a0 -= speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_3 && action == GLFW_REPEAT) {
		b0 += speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_4 && action == GLFW_REPEAT) {
		b0 -= speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}

	if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
		a1 += speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_6 && action == GLFW_PRESS) {
		a1 -= speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_7 && action == GLFW_PRESS) {
		b1 += speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_8 && action == GLFW_PRESS) {
		b1 -= speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_5 && action == GLFW_REPEAT) {
		a1 += speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_6 && action == GLFW_REPEAT) {
		a1 -= speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_7 && action == GLFW_REPEAT) {
		b1 += speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
	if (key == GLFW_KEY_8 && action == GLFW_REPEAT) {
		b1 -= speed;
		Cohen(a0, b0, a1, b1);
		Liang(a0, b0, a1, b1);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

}

int ComputeCode(double x, double y)
{
	int code = 0;
	if (y > ymax) 
		code |= TOP;
	else if (y < ymin)
		code |= BOTTOM;
	if (x > xmax) 
		code |= RIGHT;
	else if (x < xmin) 
		code |= LEFT;
	return code;
}

void Cohen(int x0, int y0, int x1, int y1) {
	int outcode0, outcode1, outcodeOut;
	double x, y;
	bool done = false;
	outcode0 = ComputeCode(x0, y0);
	outcode1 = ComputeCode(x1, y1);
	do
	{
		if (!(outcode0 | outcode1))
		{
			okay = true;
			done = true;
		}
		else if (outcode0 & outcode1) {
			done = true;
			okay = false;
		}
		else
		{
			outcodeOut = outcode0 ? outcode0 : outcode1;
			if (outcodeOut & TOP)
			{
				x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
				y = ymax;
			}
			else if (outcodeOut & BOTTOM)
			{
				x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
				y = ymin;
			}
			else if (outcodeOut & RIGHT)
			{
				y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
				x = xmax;
			}
			else
			{
				y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
				x = xmin;
			}

			if (outcodeOut == outcode0)
			{
				x0 = x;
				y0 = y;
				outcode0 = ComputeCode(x0, y0);
			}
			else
			{
				x1 = x;
				y1 = y;
				outcode1 = ComputeCode(x1, y1);
			}
		}
	} while (!done);
	if (okay) {
		double xNDC0, xNDC1, yNDC0, yNDC1;
		xNDC0 = ((float)x0 / (float)WIDTH) * 2 - 1;
		yNDC0 = ((float)y0 / (float)WIDTH) * 2 - 1;
		xNDC1 = ((float)x1 / (float)WIDTH) * 2 - 1;
		yNDC1 = ((float)y1 / (float)WIDTH) * 2 - 1;

		vertex[0] = xNDC0;
		vertex[1] = yNDC0;
		vertex[2] = xNDC1;
		vertex[3] = yNDC1;
	}
}

void Liang(int x1, int y1, int x2, int y2)
{
	float t1 = 0.0, t2 = 1.0;
	int dx = x2 - x1;
	int dy = y2 - y1;
	int i;
	float t;
	p[0] = -dx;
	q[0] = x1 - xmin1;
	p[1] = dx;
	q[1] = xmax1 - x1;
	p[2] = -dy;
	q[2] = y1 - ymin1;
	p[3] = dy;
	q[3] = ymax1 - y1;
	for (i = 0; i < 4; i++)
	{
		if (p[i] == 0 && q[i] < 0) {
			okay1 = false;
			return;
		}
		if (p[i] < 0.0)
		{
			t = q[i] / p[i];
			if (t > t1)
			{
				t1 = t;
			}
		}
		if (p[i] > 0.0)
		{
			t = q[i] / p[i];
			if (t < t2)
			{
				t2 = t;
			}
		}
	}
	if (t1 < t2)
	{
		e = x1 + t1 * (x2 - x1);
		f = y1 + t1 * (y2 - y1);
		g = x1 + t2 * (x2 - x1);
		h = y1 + t2 * (y2 - y1);
		double xNDC0, xNDC1, yNDC0, yNDC1;
		xNDC0 = ((float)e / (float)WIDTH) * 2 - 1;
		yNDC0 = ((float)f / (float)HEIGHT) * 2 - 1;
		xNDC1 = ((float)g / (float)WIDTH) * 2 - 1;
		yNDC1 = ((float)h / (float)HEIGHT) * 2 - 1;
		vertex1[0] = xNDC0;
		vertex1[1] = yNDC0;
		vertex1[2] = xNDC1;
		vertex1[3] = yNDC1;
		okay1 = true;
	}
	else {
		okay1 = false;
	}

}