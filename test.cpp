#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <iostream>
#include "basics.h"
#include "camera.h"

Camera* my_camera;

void draw()
{
	float dx = 1.0 / 800;
	float dy = 1.0 / 600;
	GLfloat x = 0.0;
	GLfloat y = -0.0;
	glBegin(GL_POINTS);
	for (int i = 0; i < 600; i++)
	{
		x = -0.0f;
		for (int j = 0; j < 800; j++)
		{
			Color c = my_camera->screen[i][j];
			glColor3f(c.X(), c.Y(), c.Z());
			glVertex2f(x, y);
			x += dx;
		}
		y += dy;
	}
	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	draw();
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(80.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void initRender()
{
	glClearColor(0, 0, 0, 0);
	my_camera = new Camera();
	my_camera->render();
}

int main(int avgc, char** avgv)
{
	glutInit(&avgc, avgv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(800, 600);
	glutCreateWindow("RayTracing");
	initRender();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}
