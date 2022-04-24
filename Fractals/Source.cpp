#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <irrKlang.h>
#include <SOIL2/SOIL2.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#pragma comment(lib, "irrKlang.lib")

using namespace std;
using namespace irrklang;

#define WIDTH 768
#define HEIGHT 768

#define PI 3.14159265358

using namespace std;

struct point {
	double x;
	double y;
}p;

int main(int argc, char* argv[]) {

	int max_depth = 10;
	if (argc > 1) {
		max_depth = atoi(argv[1]);
	}

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Dragon Curve!", NULL, NULL);

	if (!window) {
		return -1;
	}

	glfwMakeContextCurrent(window);
	glOrtho(0, WIDTH, 0, HEIGHT, -1.0, 1.0);

	while (true) {
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		vector<point> points;

		points.push_back({ p.x = WIDTH / 4.0, p.y = HEIGHT / 2.0 });
		points.push_back({ p.x = WIDTH / 2.0, p.y = HEIGHT / 4.0 });
		points.push_back({ p.x = WIDTH * 3.0 / 4.0, p.y = HEIGHT / 2.0 });
		double length = sqrt((WIDTH * WIDTH / 16.0) + (HEIGHT * HEIGHT / 16.0));
		for (int d = 0; d < max_depth; d++) {
			length = length / sqrt(2.0);
			vector<point> new_points;
			for (int i = 0; i < points.size() - 1; i++) {
				double x1 = points[i].x;
				double y1 = points[i].y;
				double theta = atan2(points[i + 1].y - y1, points[i + 1].x - x1);
				double length = sqrt(pow(points[i + 1].x - x1, 2.0) + pow(points[i + 1].y - y1, 2.0)) / sqrt(2.0);
				double delta_angle = ((i % 2) == 0 ? (PI / 4.0) : -(PI / 4.0));
				double rx = x1 + length * cos(theta + delta_angle);
				double ry = y1 + length * sin(theta + delta_angle);
				new_points.push_back(points[i]);
				new_points.push_back({ p.x = rx, p.y = ry });
			}
			new_points.push_back(points[points.size() - 1]);
			points = new_points;
		}
		glVertex2i(points[0].x, points[0].y);
		for (int i = 1; i < points.size() - 1; i++) {
			glVertex2i(points[i].x, points[i].y);
			glVertex2i(points[i].x, points[i].y);
		}
		glVertex2i(points[points.size() - 1].x, points[points.size() - 1].y);

		cout << chrono::duration_cast<chrono::duration<double>>
			(chrono::high_resolution_clock::now() - start).count() << "\n";
		glEnd();
		glfwSwapBuffers(window);
		this_thread::sleep_for(chrono::milliseconds(100));
	}

	return 0;
}