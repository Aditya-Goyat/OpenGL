#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <irrKlang.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <vector>

#pragma comment(lib, "irrKlang.lib")

using namespace std;
using namespace irrklang;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
float binomial_coff(float n, float k);
int factorial(int n);
void drawBeziervector(vector<double> x, vector<double> y, vector<float>& vert);
void FloodFill(int x, int y, float oldColor[3]);

const GLuint WIDTH = 800, HEIGHT = 600;
int cycle = 0;
int colored[550][600];
vector<float> vert1;
bool okay = false;

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, 0.0f, 1.0);\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

const GLchar* vertexShaderSource1 = "#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"layout (location = 1) in vec3 inColor;\n"
"out vec3 Color;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0f, 1.0f);\n"
"Color = inColor;\n"
"}\0";

const GLchar* fragmentShaderSource1 = "#version 330 core\n"
"out vec4 color;\n"
"in vec3 Color;\n"
"void main()\n"
"{\n"
"color = vec4(Color, 1.0f);\n"
"}\n\0";

GLuint VAO1, VBO1;

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

    glGetShaderiv(vertexShader1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);

    vector<float> vert;

    GLuint VBO, VAO;

    vector<double> x1 = { 400, 250, 250, 550, 550, 400 }, y1 = { 600, 400, 200, 200, 400, 600 };    //Center Petal.
    vector<double> x2 = { 450, 480, 550 }, y2 = { 520, 580, 600 };                                  //Right1 Top.
    vector<double> x4 = { 550, 450, 700, 430 }, y4 = { 600, 525, 300, 294 };                        //Right1 S. 
    vector<double> x6 = { 530, 560, 650 }, y6 = { 500, 570, 600 };                                  //Right2 Petal.
    vector<double> x8 = { 650, 550, 750, 490 }, y8 = { 600, 525, 330, 300 };                        //Right2 S. 
    vector<double> x3 = { 350, 320, 250 }, y3 = { 520, 580, 600 };                                  //Left1 Petal.
    vector<double> x5 = { 250, 350, 100, 370 }, y5 = { 600, 525, 300, 294 };                        //Left1 S.
    vector<double> x7 = { 270, 240, 150 }, y7 = { 500, 570, 600 };                                  //Left2 Petal.
    vector<double> x9 = { 150, 250, 50, 310 }, y9 = { 600, 525, 330, 300 };                         //Left2 S.

    vector<double> x10 = { 490, 570, 370, 450, 400 }, y10 = { 300, 250, 250, 220, 215 };            //Bottom Petal Right
    vector<double> x12 = { 580, 625, 645 }, y12 = { 325, 315, 280 };                                //Right Top
    vector<double> x14 = { 645, 590, 500, 465 }, y14 = { 280, 300, 200, 250 };                      //Right S
    vector<double> x11 = { 310, 230, 430, 350, 400 }, y11 = { 300, 250, 250, 220, 215 };            //Bottom Petal Left
    vector<double> x13 = { 220, 185, 155 }, y13 = { 325, 315, 280 };                                //Left Top
    vector<double> x15 = { 155, 210, 300, 335 }, y15 = { 280, 300, 200, 250 };                      //Left S

    vector<double> x16 = { 400, 420, 380 }, y16 = { 215, 134, 50 };

    drawBeziervector(x1, y1, vert);
    drawBeziervector(x2, y2, vert);
    drawBeziervector(x3, y3, vert);
    drawBeziervector(x4, y4, vert);
    drawBeziervector(x5, y5, vert);
    drawBeziervector(x6, y6, vert);
    drawBeziervector(x7, y7, vert);
    drawBeziervector(x8, y8, vert);
    drawBeziervector(x9, y9, vert);
    drawBeziervector(x10, y10, vert);
    drawBeziervector(x11, y11, vert);
    drawBeziervector(x12, y12, vert);
    drawBeziervector(x13, y13, vert);
    drawBeziervector(x14, y14, vert);
    drawBeziervector(x15, y15, vert);
    drawBeziervector(x16, y16, vert);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (vert.size() * sizeof(float)), &vert[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, vert.size() / 2);

        glBindVertexArray(0);

        if (okay) {
            glBindVertexArray(VAO1);
            glBindBuffer(GL_ARRAY_BUFFER, VBO1);
            glBufferData(GL_ARRAY_BUFFER, (vert1.size() * sizeof(float)), &vert1[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glUseProgram(shaderProgram1);
            glBindVertexArray(VAO1);

            glDrawArrays(GL_POINTS, 0, vert1.size() / 5);
        }

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        for (float val : vert1)
            cout << val << endl;
    }

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        cout << xpos << " " << ypos << endl;;
        float oldColor[3] = { 1.0f, 1.0f, 1.0f };
        FloodFill(xpos, abs(600-ypos), oldColor);
        okay = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        cycle++;
}

int factorial(int n)
{
    if (n <= 1)
        return(1);
    else
        n = n * factorial(n - 1);
    return n;
}

float binomial_coff(float n, float k)
{
    float ans;
    ans = float(factorial(n) / (factorial(k) * factorial(n - k)));
    return ans;
}

void drawBeziervector(vector<double> x, vector<double> y, vector<float>& vert) {
    float ytNDC, xtNDC;
    int size = x.size();
    for (double t = 0.0; t <= 1.0; t += 0.001)
    {
        float xt = 0, yt = 0;
        for (int i = 0; i < size; i++)
        {
            xt = xt + binomial_coff((float)(size - 1), (float)i) * pow(t, (double)i) * pow((1 - t), (size - 1 - i)) * x[i];
            yt = yt + binomial_coff((float)(size - 1), (float)i)* pow(t, (double)i)* pow((1 - t), (size - 1 - i))* y[i];
        }
        xtNDC = (xt / WIDTH) * 2 - 1;
        ytNDC = (yt / HEIGHT) * 2 - 1;
        vert.push_back(xtNDC);
        vert.push_back(ytNDC);
    }
}

void FloodFill(int x, int y, float oldColor[3]) {
    float color[3], xNDC, yNDC;
    glReadPixels(x, y, 1.0, 1.0, GL_RGB, GL_FLOAT, color);
    cout << x << " " << y << endl;
    if (color[0] == oldColor[0] && color[1] == oldColor[1] && color[2] == oldColor[2] && (colored[x][y] == 0))
    {
        cout << 1 << endl;
        xNDC = ((float)x / (float)WIDTH) * 2 - 1;
        yNDC = ((float)y / (float)HEIGHT) * 2 - 1;
        vert1.push_back(xNDC);
        vert1.push_back(yNDC);
        if ((cycle % 2) == 0) {
            cout << "orange" << endl;
            vert1.push_back(1.0);
            vert1.push_back(0.65);
            vert1.push_back(0.0);
        }
        else {
            cout << "green" << endl;
            vert1.push_back(0.0);
            vert1.push_back(1.0);
            vert1.push_back(0.0);
        }

        colored[x][y] = 1;
        FloodFill((x + 1), y, oldColor);
        FloodFill(x, (y + 1), oldColor);
        FloodFill((x - 1), y, oldColor);
        //FloodFill(x, (y - 1), oldColor);
        //FloodFill((x + 1), y + 1, oldColor, vert);
        //FloodFill((x - 1), y - 1, oldColor, vert);
        //FloodFill(x - 1, (y + 1), oldColor, vert);
        //FloodFill(x + 1, (y - 1), oldColor, vert);
    }
    return;
}