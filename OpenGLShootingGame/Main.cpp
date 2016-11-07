#include <iostream>
#include <vector>
#include <chrono>
#include <gl/glut.h>

#define PI 3.14159

using namespace std;
using namespace chrono;

struct Point {
	float x;
	float y;
};

time_point<system_clock> START_TIME;
time_point<system_clock> PREVIOUS_TIME;
time_point<system_clock> CURRENT_TIME;
duration<double> TIME; // time duration since the program is loaded
duration<double> TIME_DELTA;
int W = 800;
int H = 600;
float PLAYER_SPEED = 500.0f;

vector<Point> points;
Point playerPosition;
Point playerSpeed;
float playerAngle = 0;

double time() {
	return TIME.count(); // returns time since game loaded in seconds
}

double timeDelta() {
	return TIME_DELTA.count();
}

void setDefaultColor() {
	glColor3f(0, 0, 0);
}

// can also draw ellipse too
void drawCircle(int glPrimitive, Point radius) {
	int rounds = radius.x + radius.y; // how precise the circle is, this number is made up
	float factor = 2 * PI / rounds;
	
	glBegin(glPrimitive);
	for (int i = 0; i < rounds; i++) {
		float theta = i * factor;
		glVertex2f(radius.x*cosf(theta), radius.y*sinf(theta));
	}
	glEnd();
}

void drawRect(int glPrimitve, float w, float h) {
	// pivot is at the base
	glBegin(glPrimitve);
	glVertex2f(-w / 2.0f, 0);
	glVertex2f(w / 2.0f, 0);
	glVertex2f(w / 2.0f, h);
	glVertex2f(-w / 2.0f, h);
	glEnd();
}

void drawPlayer(float rad, Point gunSize) {
	glPushMatrix();
	glTranslatef(playerPosition.x, playerPosition.y, 0);
	drawCircle(GL_POLYGON, { rad, rad });
	glRotatef(playerAngle - 90, 0, 0, 1);
	drawRect(GL_LINE_LOOP, gunSize.x, gunSize.y);
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(-W/2, W/2, -H/2, H/2);

	setDefaultColor();
	glPointSize(10);
	glBegin(GL_POINTS);
	for (size_t i = 0; i < points.size(); i++)
	{
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();
	drawPlayer(25, { 10, 60 });
	glutSwapBuffers();
}

Point screenToWorld(int x, int y) {
	float sx = x - W / 2.0f;
	float sy = (H - y) - H / 2.0f;
	return{ sx, sy };
}

void click(int btn, int st, int x, int y) {
	if (st == GLUT_DOWN) {
		Point p = screenToWorld(x, y);
		cout << "Clicked at: " << p.x << " " << p.y << endl;
		points.push_back({ p.x, p.y });
	}
}

void update() {
	static int framesDrawn = 0;
	static int lastTime = 0;
	PREVIOUS_TIME = CURRENT_TIME;
	CURRENT_TIME = system_clock::now();
	TIME = CURRENT_TIME - START_TIME;
	TIME_DELTA = CURRENT_TIME - PREVIOUS_TIME;
	if ((int)time() > lastTime) {
		lastTime = (int)time();
		cout << "Average FPS: " << (float) framesDrawn / lastTime << endl;
	}
	playerPosition.x += playerSpeed.x * timeDelta();
	playerPosition.y += playerSpeed.y * timeDelta();
	glutPostRedisplay();
	framesDrawn++;
}

void reshape(int w, int h) {
	cout << "Please don't reshape my fragile window!" << endl;
	glutReshapeWindow(W, H);
}

void pointTowards(int x, int y) {
	Point p = screenToWorld(x, y);
	// make player gun points towards the mouse cursor
	playerAngle = atan2f(p.y - playerPosition.y, p.x - playerPosition.x) * 180 / PI;
}

void passiveMotion(int x, int y) {
	pointTowards(x, y);
}

void keyboard(unsigned char c, int x, int y) {
	if (c == 'a') {
		playerSpeed.x = -PLAYER_SPEED;
	}
	else if (c == 'd') {
		playerSpeed.x = PLAYER_SPEED;

	}
	else if (c == 'w') {
		playerSpeed.y = PLAYER_SPEED;

	}
	else if (c == 's') {
		playerSpeed.y = -PLAYER_SPEED;
	}
	pointTowards(x, y);
}

void keyboardUp(unsigned char c, int x, int y) {
	if (c == 'a') {
		playerSpeed.x += PLAYER_SPEED;
	}
	else if (c == 'd') {
		playerSpeed.x -= PLAYER_SPEED;

	}
	else if (c == 'w') {
		playerSpeed.y -= PLAYER_SPEED;

	}
	else if (c == 's') {
		playerSpeed.y += PLAYER_SPEED;
	}
	pointTowards(x, y);
}

void initialize() {
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutMouseFunc(click);
	glutReshapeFunc(reshape);
	glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	points.push_back({ 0, 0 });
	START_TIME = system_clock::now();
	CURRENT_TIME = system_clock::now();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Shooting Game by Off");
	initialize();
	glutMainLoop();
	return 0;
}