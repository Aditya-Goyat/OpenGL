#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <math.h>

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void DDA(double x1, double z1, double x2, double y2, vector<float>& vert);

const GLuint WIDTH = 800, HEIGHT = 800;

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, 0.0f, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"uniform vec4 inColor;\n"
"void main()\n"
"{\n"
"color = inColor;\n"
"}\n\0";

double x1, z1, x2, y2, flag = 0;
bool shouldDisplay = false, okay = false;
int cycle = 0;

int main()
{
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

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vector<float> vert;

    GLuint VBO, VAO;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (okay) {
            DDA(x1, z1, x2, y2, vert);

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, (vert.size() * sizeof(float)), &vert[0], GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            okay = false;
        }

        if (shouldDisplay) {
            glUseProgram(shaderProgram);
            int vertexColorLocation = glGetUniformLocation(shaderProgram, "inColor");
            if ((cycle % 3) == 0)
                glUniform4f(vertexColorLocation, 1.0, 0.0, 0.0, 1.0f);
            else if ((cycle % 3) == 1)
                glUniform4f(vertexColorLocation, 0.0, 1.0, 0.0, 1.0f);
            else if ((cycle % 3) == 2)
                glUniform4f(vertexColorLocation, 0.0, 0.0, 1.0, 1.0f);
            glBindVertexArray(VAO);
            glDrawArrays(GL_POINTS, 0, vert.size() / 2);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        shouldDisplay = true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (flag == 0) {
            glfwGetCursorPos(window, &x1, &z1);
            flag++;
        }
        else {
            glfwGetCursorPos(window, &x2, &y2);
            flag = 0;
            okay = true;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        cycle++;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &x2, &y2);
        okay = true;
    }
}

void DDA(double x1, double y1, double x2, double y2, vector<float>& vert) {
    float xNDC, yNDC;
    y1 = abs(800 - y1);
    y2 = abs(800 - y2);
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int p = (2 * dy) - dx;

    int x = x1;
    int y = y1;

    if (dx >= dy) {
        if (x2 < x1) {
            while (x >= x2) {
                if (p >= 0) {
                    if (y2 > y1)
                        y++;
                    else
                        y--;
                    p = p + (2 * dy) - (2 * dx);
                }
                else {
                    p = p + (2 * dy);
                }
                xNDC = ((float)x / (float)WIDTH) * 2 - 1;
                yNDC = ((float)y / (float)HEIGHT) * 2 - 1;
                vert.push_back(xNDC);
                vert.push_back(yNDC);
                x--;
            }
        }
        else {
            while (x <= x2) {
                if (p >= 0) {
                    if (y2 > y1)
                        y++;
                    else
                        y--;
                    p = p + (2 * dy) - (2 * dx);
                }
                else {
                    p = p + (2 * dy);
                }
                xNDC = ((float)x / (float)WIDTH) * 2 - 1;
                yNDC = ((float)y / (float)HEIGHT) * 2 - 1;
                vert.push_back(xNDC);
                vert.push_back(yNDC);
                x++;
            }
        }
    }
    else {
        if (y2 > y1) {
            while (y <= y2) {
                if (p >= 0) {
                    if (x2 < x1)
                        x--;
                    else
                        x++;
                    p = p + (2 * dx) - (2 * dy);
                }
                else {
                    p = p + (2 * dx);
                }
                xNDC = ((float)x / (float)WIDTH) * 2 - 1;
                yNDC = ((float)y / (float)HEIGHT) * 2 - 1;
                vert.push_back(xNDC);
                vert.push_back(yNDC);
                y++;
            }
        }
        else {
            while (y >= y2) {
                if (p >= 0) {
                    if (x2 < x1)
                        x--;
                    else
                        x++;
                    p = p + (2 * dx) - (2 * dy);
                }
                else {
                    p = p + (2 * dx);
                }
                xNDC = ((float)x / (float)WIDTH) * 2 - 1;
                yNDC = ((float)y / (float)HEIGHT) * 2 - 1;
                vert.push_back(xNDC);
                vert.push_back(yNDC);
                y--;
            }
        }
    }
}