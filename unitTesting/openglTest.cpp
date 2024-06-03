#include <windows.h>
#include <WinUser.h>

#include <GL/glew.h>
#include <iostream>
#include <string>

#include "window.h"
#include "gl_context.h"
#include "unitTesting.h"
// Paint the title on the custom frame.
extern int init();
namespace UnitTesting {


void drawOpenGL3(int sizeX, int sizeY) {
	glViewport(0, 0, sizeX, sizeY);
	glClearColor(0.5, 0.05, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void drawOpenGL1(NWin::winHandle h) {
	NWin::Window* win = NWin::Window::stGetWindow(h);
	glClearColor(0.3, 0.3, 0.3, 0.0);
	NWin::Vec2 size;
	win->getDrawAreaSize(size);
	glViewport(0, 0, size.x, size.y);
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



void drawOpenGL3(NWin::winHandle h) {
	NWin::Window* win = NWin::Window::stGetWindow(h);
	NWin::Vec2 size;
	win->getDrawAreaSize(size);
	drawOpenGL3(size.x, size.y);
	win->swapBuffers();
}



void testOpenGL3Window() {
	NWin::Window* w;
	NWin::WindowCrtInfo c{};
	NWin::OpenGLInfo glInfo;
	NWin::GlContext context;

	c.metrics.pos = { 0,0 };
	c.description = "HelloWorld";
	c.metrics.size = { 480, 360 };
	c.style = (NWin::Word)NWin::WindowStyle::Default;
	c.exStyle = (NWin::Word)NWin::WindowExStyle::Default;
	w = NWin::Window::stCreateWindow(c);


	w->dwmDarkModeFrame(1);
	w->dwmDontRoundCorners(0);
	//Context------------------
	glInfo.minVersion = 3;
	glInfo.maxVersion = 3;
	context.create(w, glInfo);
	context.makeCurrent();
	if (glewInit() != GLEW_OK) {
		std::cout <<"glew problem" << std::endl; 
	}

	//Render Loop--------------
	init();
	while (w->shouldLoop()) { 
		w->update(); 
		drawOpenGL3(w->_getHandle()); 
		context.setCurCtxVSync(1);
		w->swapBuffers();
	}
	NWin::Window::stClean(w);
	context.makeCurrent(1);
}

};