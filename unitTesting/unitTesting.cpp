#include "unitTesting.h"
#include <windows.h>
#include "window.h"
#include "gl_context.h"


namespace UnitTesting {

void resize(NWin::winHandle handle, NWin::Vec2 newSize) {
		//printf("ResizeTest\n");
}

void simpleWindow() {
	NWin::Window* w;
	NWin::WindowCrtInfo c{};
	NWin::OpenGLInfo glInfo;
	NWin::GlContext context;

	c.metrics.pos = { 0,0 };
	c.description = "HelloWorld";
	c.metrics.size = { 480, 360 };
	c.style = (NWin::Word)NWin::WindowStyle::Default;
	c.exStyle = (NWin::Word)NWin::WindowExStyle::Default;
    c.customWindowProcPtr = (void*)NWin::defaultWinProc;
    
	w = NWin::Window::stCreateWindow(c);
	w->setResizeCallback(&resize);
	w->dwmDarkModeFrame(1);
	w->dwmDontRoundCorners(0);
	//Context------------------
	glInfo.minVersion = 3;
	glInfo.maxVersion = 3;
	context.create(w, glInfo);
	context.makeCurrent();
	//Render Loop--------------
	while (w->shouldLoop()) { w->update(); }
	NWin::Window::stClean(w);
	context.makeCurrent(1);
	}
};
