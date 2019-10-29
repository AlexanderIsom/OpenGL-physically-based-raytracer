
#include <GL/glut.h>
#include <math.h>

static int win(0);

const GLuint Width = 620;
const GLuint Height = 480;

float pos = -10.0f;

void draw() {

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	//int i = 0;

	glBegin(GL_POLYGON);
	//glVertex2i(i, Height / 2 + sin(i * 50) * 100);
	glVertex2f(pos, 1.0);
	glVertex2f(pos, -1.0);
	glVertex2f(pos+2.0, -1.0);
	glVertex2f(pos+2.0, 1.0);
	glEnd();
	glutSwapBuffers();
	//Sleep(50);

//i++;

}

void Timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000/ 60, Timer, 0);
	pos += 0.15f;
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(Width, Height);
	glutCreateWindow("Test");

	glViewport(0, 0, (GLsizei)0, (GLsizei)0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);

	glutDisplayFunc(draw);
	glutTimerFunc(1000, Timer,0);
	glClearColor(0, 0, 0, 0);

	//std::thread first(draw, Height / 4);
	//std::thread second(draw, Height - Height / 1);

	//first.join();
	//second.join();
	glutMainLoop();
}