#include <iostream>
#include "window.h"

void resize(NWin::winHandle handle) {
	printf("ResizeTest\n");
}

void sample() {
	NWin::Window* w;
	NWin::WindowCrtInfo c{};
	c.metrics.pos = { 0,0 };
	c.description = "HelloWorld";
	c.metrics.size = { 480, 360 };

	w = NWin::Window::stCreateWindow(c);
	w->resizeCallback = &resize;
	while (w->shouldLoop()) { w->update(); }
	NWin::Window::stDestroyWindow(w);
}

int main() {
	sample();
}
