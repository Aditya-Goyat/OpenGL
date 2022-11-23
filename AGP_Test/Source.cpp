#include <stdlib.h>
#include <stdlib.h>
#include <glut.h>
#include <GL/glu.h>
#include <iostream>
#include <string>
#include <math.h>

#define LBoundry -1
#define RBoundry 1
#define BBoundry -1
#define TBoundry 1
int score = 0;
bool upd = true;
float gameTime = 10000.0f, timeLeft = gameTime;

using namespace std;

void GameOver();

class Rect
{
protected:
	float x, y, h, w;
	float velocityX, velocityY;
	bool isPointInsideRect(float a, float b)
	{
		if (a >= x && a <= x + w)
			if (b >= y && b <= y + h)
				return true;
		return false;
	}
	// float cr,cg,cb;
public:
	Rect(float X, float Y, float H, float W, float vx = 0, float vy = 0)
	{
		x = X;
		y = Y;
		h = H;
		w = W;
		velocityX = vx;
		velocityY = vy;
	}
	void setColor(float R, float G, float B)
	{
		glColor3f(R, G, B);
	}
	void setVelocity(float xx, float yy)
	{
		velocityX = xx;
		velocityY = yy;
	}
	void display()
	{
		x += velocityX;
		y += velocityY;
		glBegin(GL_POLYGON);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
	}
	bool isCollide(Rect r1)
	{
		if (isPointInsideRect(r1.x, r1.y) || isPointInsideRect(r1.x + r1.w, r1.y) || isPointInsideRect(r1.x + r1.w, r1.y + r1.h) || isPointInsideRect(r1.x, r1.y + r1.h))
		{
			if (upd == true) {

				score++;
				upd = false;
			}

			return true;
		}
		else
			return false;
	}

};

class mainRect : public Rect
{
	float speed = .03;
public:
	mainRect(float X, float Y, float H, float W, float vx = 0, float vy = 0) : Rect(X, Y, H, W, vx, vy)
	{

	}
	void display()
	{
		if (x <= LBoundry)
			setVelocity(0.001, 0);
		if (x + w >= RBoundry)
			setVelocity(-0.001, 0);
		if (y <= BBoundry)
			setVelocity(0, 0.001);
		if (y + h >= TBoundry)
			setVelocity(0, -0.001);
		Rect::display();
	}
	void MoveRight()
	{

		setVelocity(speed, 0);
	}
	void MoveLeft()
	{
		setVelocity(-speed, 0);
	}
	void MoveUp()
	{
		setVelocity(0, speed);
	}
	void MoveDown()
	{
		setVelocity(0, -speed);
	}
	void stop()
	{
		setVelocity(0, 0);
	}
};

class BoundingRect : public Rect
{
public:
	void display()
	{
		int flag = touchingBoundry();
		if (flag)
		{
			if (flag == -1)
				velocityY = -velocityY;
			if (flag == 1)
				velocityX = -velocityX;
		}

		Rect::display();
	}
	void random(float& X, float& Y) {
		srand(time(NULL));
		int mx = 1;
		int my = 1;
		int mi = -1;
		X = mi + (rand() % (mx - mi));
		Y = mi + (rand() % (my - mi));
	}
	int touchingBoundry()
	{
		if (x < LBoundry || x + w > RBoundry)
			return 1;
		if (y < BBoundry || y + h > TBoundry)
			return -1;
		return 0;
	}
	BoundingRect(float X, float Y, float H, float W, float vx = 0, float vy = 0) : Rect(X, Y, H, W, vx, vy)
	{

	}

	~BoundingRect() {


	}
};

mainRect pc = mainRect(-0.0, -0.0, 0.20, 0.20, 0, 0);
BoundingRect r1 = BoundingRect(-0.5, -0.5, 0.15, 0.15, 0.01, .02);
//BoundingRect r2 = BoundingRect(-0.5, -0.5, 0.25, 0.25, -0.02, .01);

void timeOut(int a)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 100, timeOut, 0);
}
void init()
{
	glViewport(0, 0, 500, 500);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(LBoundry, RBoundry, BBoundry, TBoundry);
}

void keypress(unsigned char key, int x, int y)
{
	if (key == 'a')
	{

		pc.MoveLeft();
	}

	if (key == 'd')
	{
		pc.MoveRight();
	}

	if (key == 'w')
	{
		pc.MoveUp();
	}

	if (key == 's')
	{
		pc.MoveDown();
	}

}
void keyUpFunction(unsigned char key, int x, int y)
{
	pc.stop();
}

void GameOver() {
	string gameover = "Game Over!";

	int n = gameover.size();

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(-0.3, 0.0);
	for (int i = 0; i < n; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, gameover[i]);
	}

	glFlush();
	glutSwapBuffers;
}

void DisplayTimer() {
	glColor3f(1.0f, 1.0f, 1.0f);
	timeLeft = gameTime - glutGet(GLUT_ELAPSED_TIME);
	string p = to_string(ceil(timeLeft/1000.0f));
	string s = "Time Left : " + p;
	int n = s.size();
	glRasterPos2f(0.5, 0.85);
	for (int i = 0; i < n - 7; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
	}

	glFlush();
	glutSwapBuffers;
}

void DisplayScore() {
	glRasterPos2f(-0.95, 0.85);
	glColor3f(1.0f, 1.0f, 1.0f);
	string p = to_string(score);
	string s = "Score : " + p;
	int n = s.size();
	glRasterPos2f(-0.95, 0.85);
	for (int i = 0; i < n; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, s[i]);
	}

	glFlush();
	glutSwapBuffers;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	float currentTime = glutGet(GLUT_ELAPSED_TIME);
	if (currentTime > gameTime) {
		GameOver();
		DisplayScore();
		return;
	}

	r1.setColor(1.0, 0, 0);
	if (!pc.isCollide(r1)) {
		upd = true;
		r1.display();

	}
	else {

	}

	// r2.setColor(0,1.0,0);
	// r2.display();
	glColor3f(1.0f, 0.0f, 0.0f);

	// pc.MoveLeft();
	// pc.setVelocity(.03,0);
	pc.display();
	glFlush();
	
	DisplayTimer();

	DisplayScore();

	glEnd();
	glFlush();
}

/* Main method */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	int mode = GLUT_RGB | GLUT_SINGLE;
	glutInitWindowSize(500, 500);
	glutInitDisplayMode(mode);
	glutCreateWindow("Rect Class");
	init();
	glutDisplayFunc(&display);
	glutKeyboardFunc(&keypress);
	glutKeyboardUpFunc(&keyUpFunction);
	glutTimerFunc(1000 / 100, timeOut, 0);
	glutMainLoop();

	return EXIT_SUCCESS;
}