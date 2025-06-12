#include <windows.h>
#include <WinUser.h>

//#include <GL/glew.h>
#include <GL/GL.h>
#include <iostream>
#include <string>

#include "nwindow.h"
#include "gl_context.h"


#include "unitTesting.h"
// Paint the title on the custom frame.
// Init would be a function that initializes and binds VAO and shaders.
//extern int init();
namespace UnitTesting {


//void drawOpenGL3(int sizeX, int sizeY) {
//	glViewport(0, 0, sizeX, sizeY);
//	glClearColor(0.5, 0.05, 0.1, 1);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
//}

//void drawOpenGL3(NWin::winHandle h) {
//	NWin::NWindow* win = NWin::GetWin(h);
//	NWin::Vec2 size;
//	win->getDrawAreaSize(size);
//	drawOpenGL3(size.x, size.y);
//	win->swapBuffers();
//}

void drawOpenGL1(NWin::winHandle h) {
	NWin::NWindow* win = NWin::GetWin(h);
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


void testOpenGL3Window() {
	NWin::NWindow* w;
	NWin::WinCrtInfo c{};
	NWin::OpenGLInfo glInfo;
	NWin::GlContext context;

	c.description = "HelloWorld";
	c.metrics.size     = { 720, 480 };
	w = NWin::CreateWin(c);

	//Context------------------
	glInfo.minVersion = 3;
	glInfo.maxVersion = 3;
	context.create(w, glInfo);
	context.makeCurrent();
//	if (glewInit() != GLEW_OK) {
//		std::cout <<"glew problem" << std::endl; 
//	}

	//Render Loop--------------
	bool fullscreen = 0;
	while (w->_shouldLoop) { 
		if (w->getKeyboard().onKeyPress((NWin::Key)'F')) {
			w->enableFullScreen();
		}
		
		if (w->getKeyboard().onKeyPress((NWin::Key)'G')) {
			w->disableFullscreen();
		}
		w->update(); 
		//drawOpenGL3(w->_handle); 
		context.setCurCtxVSync(1);
		w->swapBuffers();
	}
	NWin::DestroyWin(w);
	context.makeCurrent(1);
}

};
