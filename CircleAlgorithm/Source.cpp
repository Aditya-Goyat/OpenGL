#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <vector>
#include <math.h>

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void Circle(double x1, double z1, double x2, double y2, vector<float>& vert);
void Ellipse(double x1, double y1, double x2, double y2, double x3, double y3, vector<float>& vert);

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

const GLchar* vertexShaderSource1 = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout(location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position, 1.0);\n"
"TexCoord = aTexCoord;\n"
"}\0";

const GLchar* fragmentShaderSource1 = "#version 330 core\n"
"out vec4 color;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"color = texture(ourTexture, TexCoord);\n"
"}\n\0";

double x1, z1, x2, y2, flag = 0, elflag = 0;
double c1, c2, m1, m2, d1, d2;
bool shouldDisplay = false, okay = false, ellipse = false;
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

    float vertices[] = {
        // positions          // texture coords
        -0.5f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
        -0.5f, 0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
        -1.0f, 0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
        -1.0f, 1.0, 0.0f,    0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO1, VAO1, EBO;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

        glUseProgram(shaderProgram1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (okay) {
            if (ellipse)
                Ellipse(c1, c2, m1, m2, d1, d2, vert);
            else
                Circle(x1, z1, x2, y2, vert);

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
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgram1);

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        shouldDisplay = true;
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        ellipse = !ellipse;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (ellipse) {
            if (elflag == 0) {
                glfwGetCursorPos(window, &c1, &c2);
                elflag++;
            }
            else if (elflag == 1) {
                glfwGetCursorPos(window, &m1, &m2);
                elflag++;
            }
            else {
                glfwGetCursorPos(window, &d1, &d2);
                elflag = 0;
                okay = true;
            }
        }
        else {
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
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        cycle++;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &x2, &y2);
        okay = true;
    }
}

void Circle(double x1, double y1, double x2, double y2, vector<float>& vert) {
    float xNDC, yNDC;
    y1 = abs(800 - y1);
    y2 = abs(800 - y2);
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    
    float equation = abs(pow(dx, 2) - pow(dy, 2));
    int radius = (int)sqrt(equation);

    float d = 5.0 / 4.0 - radius;

    float x = 0, y = radius;

    while (y > x) {
        x++;
        if (d < 0) {
            d += 2 * (x + 1) + 1;
        }
        else {
            y--;
            d += 2 * (x - y) + 1;
        }
        
        xNDC = ((float)(x + x1) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y + y1) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(1.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 - x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y + y1) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(1.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x + x1) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 - y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(1.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 - x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 - y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(1.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 + y) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 + x) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(1.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 - y) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 + x) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(1.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 + y) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 - x) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(1.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 - y) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 - x) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(1.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);
    }

}

void Ellipse(double x1, double y1, double x2, double y2, double x3, double y3, vector<float>& vert) {
    float xNDC, yNDC;
    y1 = abs(800 - y1);
    y2 = abs(800 - y2);
    y3 = abs(800 - y3);
    
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    float equation = abs(pow(dx, 2) - pow(dy, 2));
    int ry = (int)sqrt(equation);

    dx = abs(x3 - x1);
    dy = abs(y3 - y1);
    equation = abs(pow(dx, 2) - pow(dy, 2));
    int rx = (int)sqrt(equation);

    float x = 0;
    float y = ry;
    float p1 = ry * ry - (rx * rx) * ry + (rx * rx) * (0.25);

    dx = 2 * (ry * ry) * x;
    dy = 2 * (rx * rx) * y;

    while (dx < dy) {
        xNDC = ((float)(x1 + x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 + y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(0.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 - x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 + y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(0.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 + x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 - y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(0.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 - x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 - y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(0.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        if (p1 < 0) {
            x++;
            dx = 2 * (ry * ry) * x;
            p1 += dx + (ry * ry);
        }
        else {
            x++;
            y--;
            dx = 2 * (ry * ry) * x;
            dy = 2 * (rx * rx) * y;
            p1 += dx - dy + (ry * ry);
        }
    }

    float p2 = (ry * ry) * (x + 0.5) * (x + 0.5) + (rx * rx) * (y - 1) * (y - 1) - (rx * rx) * (ry * ry);

    while (y > 0) {
        xNDC = ((float)(x1 + x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 + y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(0.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 - x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 + y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(0.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 + x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 - y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(0.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        xNDC = ((float)(x1 - x) / (float)WIDTH) * 2 - 1;
        yNDC = ((float)(y1 - y) / (float)HEIGHT) * 2 - 1;
        vert.push_back(xNDC);
        vert.push_back(yNDC);
        vert.push_back(0.0f);
        vert.push_back(1.0f);
        vert.push_back(0.0f);

        if (p2 > 0) {
            y--;
            dy = 2 * (rx * rx) * y;;
            p2 = p2 - dy + (rx * rx);
        }
        else {
            x++;
            y--;
            dy = dy - 2 * (rx * rx);
            dx = dx + 2 * (ry * ry);
            p2 = p2 + dx - dy + (rx * rx);
        }
    }
}
