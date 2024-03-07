#include <iostream>
#include "gl_context.h"
#include "window.h"   
#include <windows.h>
//#include <gl/glu.h>
//#include <GL/GL.h>
#include <gl/glew.h>

void resize(NWin::winHandle handle) {
	//printf("ResizeTest\n");
}

void draw11(NWin::winHandle h) {
	NWin::Window* win = NWin::Window::stGetWindow(h);
	glClearColor(0.3, 0.3, 0.3, 0.0);
	glViewport(0,0, win->getDrawAreaSize().x,win->getDrawAreaSize().y);
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2i(0, 1);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2i(-1, -1);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2i(1, -1);
	glEnd();
	glFlush();
}

extern void draw(int sizeX, int sizeY);
extern int init();

void draw3(NWin::winHandle h) {
	NWin::Window* win = NWin::Window::stGetWindow(h);
	draw(win->getDrawAreaSize().x, win->getDrawAreaSize().y);
	win->swapBuffers();
}

void sample() {
	NWin::Window* w;
	NWin::WindowCrtInfo c{};
	c.metrics.pos = { 0,0 };
	c.description = "HelloWorld";
	c.metrics.size = { 480, 360 };
	c.style   = WS_OVERLAPPEDWINDOW;
	c.exStyle = WS_EX_APPWINDOW;
	w = NWin::Window::stCreateWindow(c);
	w->resizeCallback = &resize;
	w->drawCallback   = &draw3;
	NWin::GlContext context;
	context.create(w);
	context.makeCurrent();
	context.initCoreOpenGL();
	init();
	while (w->shouldLoop()) { w->update(); }
	NWin::Window::stDestroyWindow(w);
	context.makeCurrent(1);
}

int main() {
	sample();
}
