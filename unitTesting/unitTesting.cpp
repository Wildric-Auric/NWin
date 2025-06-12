#include "unitTesting.h"
#include <windows.h>
#include "nwindow.h"
#include "gl_context.h"


namespace UnitTesting {

void resize(NWin::winHandle handle, NWin::Vec2 newSize) {
		//printf("ResizeTest\n");
}

void simpleWindow() {
	NWin::NWindow* w;
	NWin::WinCrtInfo c{};
	NWin::OpenGLInfo glInfo;
	NWin::GlContext context;

	c.description = "HelloWorld";
	c.metrics.size = { 480, 360 };
    
	w = CreateWin(c);
	w->setResizeCallback(&resize);
	//w->dwmDarkModeFrame(1);
	//w->dwmDontRoundCorners(0);
	//Context------------------
	glInfo.minVersion = 3;
	glInfo.maxVersion = 3;
	context.create(w, glInfo);
	context.makeCurrent();
	//Render Loop--------------
	while (w->shouldLoop()) { w->update(); }
	DestroyWin(w);
	context.makeCurrent(1);
	}
};
