#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <irrKlang.h>
using namespace irrklang;

#pragma comment(lib, "irrKlang.lib")

using namespace std;

//Vertex Shader
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"uniform float Offset;\n"
"uniform float xNDC;\n"
"uniform float yNDC;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x*Offset + xNDC, position.y*Offset + yNDC, position.z, 1.0);\n"
"}\0";

//Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 Color;\n"
"void main()\n"
"{\n"
"   FragColor = Color;\n"
"}\n\0";

GLint width = 800, height = 800;
GLint flag = 0;
double xpos, ypos;
float xNDC, yNDC;
bool isTriangle = true;
ISoundEngine* SoundEngine = createIrrKlangDevice();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        SoundEngine->play2D("powerup.wav", false);
        isTriangle = !isTriangle;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        flag++;
        glfwGetCursorPos(window, &xpos, &ypos);
        xNDC = (xpos / width) * 2 - 1;
        yNDC = -1 * ((ypos / height) * 2 - 1);
        SoundEngine->play2D("solid.wav", false);
    }
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "500080575_AdityaGoyat_Experiment02", NULL, NULL);

    glfwMakeContextCurrent(window);

    glewInit();

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    GLuint fragmentShader, vertexShader;

    int success;
    char infoLog[512];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    GLfloat vertices[] = {
    -0.5f, -0.5f, 0.0f,         //vertex 1
     0.5f, -0.5f, 0.0f,         //vertex 2
     0.0f,  0.5f, 0.0f          //vertex 3
    };

    GLfloat verticesRect[] = {
         0.5f,  0.5f, 0.0f,  // Top Right
         0.5f, -0.5f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f   // Top Left 
    };
    GLuint indices[] = {
        0, 1, 3,  // First Triangle
        1, 2, 3   // Second Triangle
    };

    GLuint VBOs[2], VAOs[2], EBO;

    glGenVertexArrays(1, &VAOs[0]);
    glGenBuffers(1, &VBOs[0]);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glGenVertexArrays(1, &VAOs[1]);
    glGenBuffers(1, &VBOs[1]);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRect), &verticesRect, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    float xOffset = 0.0f;
    float speed = 0.01f;

    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float value = sin(glfwGetTime());
        int vertexShaderPosition = glGetUniformLocation(shaderProgram, "Offset");
        glUniform1f(vertexShaderPosition, value);
        int xNDCPosition = glGetUniformLocation(shaderProgram, "xNDC");
        glUniform1f(xNDCPosition, xNDC);
        int yNDCPosition = glGetUniformLocation(shaderProgram, "yNDC");
        glUniform1f(yNDCPosition, yNDC);
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "Color");

        if ((flag % 3) == 0)
            glUniform4f(vertexColorLocation, 1.0, 0.0, 0.0, 1.0f);
        else if ((flag % 3) == 1)
            glUniform4f(vertexColorLocation, 0.0, 1.0, 0.0, 1.0f);
        else if ((flag % 3) == 2)
            glUniform4f(vertexColorLocation, 0.0, 0.0, 1.0, 1.0f);

        if (isTriangle) {
            glBindVertexArray(VAOs[0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        else {
            glBindVertexArray(VAOs[1]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBOs[0]);
    glDeleteVertexArrays(1, &VAOs[0]);

    glfwTerminate();
    return 0;
}