#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <vector>
#include <math.h>

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Bresenham(double x1, double z1, double x2, double y2, vector<float>& vert);

const GLuint WIDTH = 800, HEIGHT = 800;

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"layout (location = 1) in vec3 inColor;\n"
"out vec3 Color;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, 0.0f, 1.0);\n"
"Color = inColor;\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"uniform float inColor;\n"
"in vec3 Color;\n"
"void main()\n"
"{\n"
"color = vec4(Color.x + inColor, Color.y + inColor, Color.z + inColor, 1.0f);\n"
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

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

    vector<float> vert;

    GLuint VBO, VAO;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramTexture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
        glBindVertexArray(VAOTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (okay) {
            Bresenham(x1, z1, x2, y2, vert);

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, (vert.size() * sizeof(float)), &vert[0], GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            okay = false;
        }

        if (shouldDisplay) {
            float timeValue = glfwGetTime() * 2;
            glUseProgram(shaderProgram);
            if ((cycle % 2) != 0) {
                float colorValue = sin(timeValue) / 2 + 0.5;
                int vertexColorLocation = glGetUniformLocation(shaderProgram, "inColor");
                glUniform1f(vertexColorLocation, colorValue);
            }
            else {
                int vertexColorLocation = glGetUniformLocation(shaderProgram, "inColor");
                glUniform1f(vertexColorLocation, 0);
            }
            glBindVertexArray(VAO);
            glDrawArrays(GL_POINTS, 0, vert.size() / 2);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAOTexture);
    glDeleteBuffers(1, &VBOTexutre);
    glDeleteBuffers(1, &EBOTexture);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgramTexture);

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

void Bresenham(double x1, double y1, double x2, double y2, vector<float>& vert) {
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
                vert.push_back(1.0f);
                vert.push_back(0.0f);
                vert.push_back(0.0f);
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
                vert.push_back(0.0f);
                vert.push_back(1.0f);
                vert.push_back(0.0f);
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
                vert.push_back(0.0f);
                vert.push_back(0.0f);
                vert.push_back(1.0f);
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
                vert.push_back(1.0f);
                vert.push_back(1.0f);
                vert.push_back(0.0f);
                y--;
            }
        }
    }
}