#include "gl_context.h"
#include <Windows.h>
#include <wingdi.h>
#include <gl/glew.h>
#include <GL/GLU.h>
#include <GL/GL.h>

#define WGL_SAMPLE_BUFFERS_ARB            0x2041
#define WGL_SAMPLES_ARB                   0x2042
#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_PIXEL_TYPE_ARB                0x2013
#define WGL_TYPE_RGBA_ARB                 0x202B
#define WGL_ACCELERATION_ARB              0x2003
#define WGL_FULL_ACCELERATION_ARB         0x2027
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_DEPTH_BITS_ARB                0x2022
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_SWAP_EXCHANGE_ARB             0x2028
#define WGL_SWAP_METHOD_ARB               0x2007
#define WGL_SWAP_COPY_ARB                 0x2029

typedef BOOL(WINAPI*  PFNWGLCHOOSEPIXELFORMATARBPROC)    (HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL(WINAPI*  PFNWGLSWAPINTERVALEXTPROC)		 (int interval);

PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr; //To disable compatibility and select certain OpenGL version only
PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = nullptr;
namespace NWin {


GlContext* GlContext::_current = nullptr;

GlContext* GlContext::getCurrent() {
	return GlContext::_current;
}

Window* GlContext::getWindow() {
	return _attachedWindow;
}

bool GlContext::isCurrent() {
	return GlContext::_current == this;
}

struct PixelFormat {
	int choice = 0; //Holds the value returned by Choose
	PIXELFORMATDESCRIPTOR pFormat;
};

static void fillpixelFormat(PIXELFORMATDESCRIPTOR& pFormat) {
	pFormat = {};
	//Fixed values by the doc, maybe for future multiple formatdescriptor types https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-pixelformatdescriptor
	pFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pFormat.nVersion = 1;
	//Other values--------------------
	pFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;  //PFD_DRAW_TO_BITMAP | PFD_SUPPORT_GDI;
	pFormat.iPixelType = PFD_TYPE_RGBA;
	pFormat.cColorBits = 32;
	pFormat.cAlphaBits = 8;
	pFormat.cDepthBits = 24;
}


static NWin::Window* www;
static PixelFormat pf; //TODO::Move this elsewhere
NWIN_GL_STATUS GlContext::create(Window* w) {

	//temp----------------------------
		www = w;
		NWin::Window* window;
		NWin::WindowCrtInfo c{};
		c.metrics.pos = { 0,0 };
		c.description = "HelloWorld";
		c.metrics.size = { 480, 360 };
		window = NWin::Window::stCreateWindow(c);
	//-------------------------

	_attachedWindow = window;
	
	fillpixelFormat(pf.pFormat);
	
	
	WIN_CHECK21(pf.choice = ChoosePixelFormat((HDC)window->_getDcHandle(), &pf.pFormat), return NWIN_GL_STATUS::PIXEL_FORMAT_CHOICE_FAILURE);
	WIN_CHECK21(SetPixelFormat((HDC)window->_getDcHandle(),1,&pf.pFormat),		return NWIN_GL_STATUS::SET_PIXEL_FORMAT_FAILURE);
	//temp----------------
	WIN_CHECK21(pf.choice = ChoosePixelFormat((HDC)w->_getDcHandle(), &pf.pFormat), return NWIN_GL_STATUS::PIXEL_FORMAT_CHOICE_FAILURE);
	WIN_CHECK21(SetPixelFormat((HDC)w->_getDcHandle(), 1, &pf.pFormat), return NWIN_GL_STATUS::SET_PIXEL_FORMAT_FAILURE);
	//--------------------
	WIN_CHECK21(_contextHandle = wglCreateContext((HDC)window->_getDcHandle()), return NWIN_GL_STATUS::CONTEXT_CREATION_FAILURE);

	return NWIN_GL_STATUS::NONE;
}

NWIN_GL_STATUS GlContext::makeCurrent(bool noContext) {
	auto a = GetLastError();
	if (noContext) { 
		wglMakeCurrent(NULL,NULL); GlContext::_current = nullptr; 
		return NWIN_GL_STATUS::NONE;
	}
	WIN_CHECK21(wglMakeCurrent((HDC)_attachedWindow->_getDcHandle(), (HGLRC)_contextHandle), \
		return NWIN_GL_STATUS::CONTEXT_MAKE_CURRENT_FAILURE
	);
	GlContext::_current = this;
	return NWIN_GL_STATUS::NONE;
}

NWIN_GL_STATUS GlContext::initCoreOpenGL() {

	//------

	WIN_CHECK(wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB"));
	WIN_CHECK(wglChoosePixelFormatARB    = (PFNWGLCHOOSEPIXELFORMATARBPROC)   wglGetProcAddress("wglChoosePixelFormatARB"));

	int attribs[] = {
	WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
	WGL_CONTEXT_MINOR_VERSION_ARB, 0, //fix this to 3 so you can't use opengl comp
	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0x2094, 0x0002, 0 // ,WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB TODO::Define these, it is to disable compatibility
	};

	int pixelAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32, 
		WGL_ALPHA_BITS_ARB, 8, 
		WGL_DEPTH_BITS_ARB, 24,
		0
	};

	Window::stDestroyWindow(_attachedWindow);
	_attachedWindow = www;
	HGLRC newC;
	UINT NUM;
	WIN_CHECK(wglChoosePixelFormatARB((HDC)_attachedWindow->_getDcHandle(), pixelAttribs, NULL, 1,&pf.choice, &NUM))
	WIN_CHECK(newC = wglCreateContextAttribsARB((HDC)_attachedWindow->_getDcHandle(), 0, attribs));
	makeCurrent(1); 
	WIN_CHECK(wglDeleteContext((HGLRC)_contextHandle));
	_contextHandle = newC;
	makeCurrent();


	int glewInitValue = glewInit();
	WIN_CHECK_COMPLETE(glewInitValue == GLEW_OK, 0, return NWIN_GL_STATUS::CORE_OPENGL_INIT_FAILURE);

	return NWIN_GL_STATUS::NONE;
}

}