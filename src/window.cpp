#include "window.h"
#include <string>
#include<Windows.h>
#include<winuser.h>

typedef LONG_PTR  (*__stdcall win_proc_ptr)(HWND, UINT, WPARAM, LPARAM);


namespace NWin {

uint64_t Window::_incID = 1;
std::unordered_map<winHandle, Window> Window::_windowsMap;


void getWinRect(const Rect& r, RECT& outRect) {
		outRect.top = r.pos.y + r.size.y * 0.5;
		outRect.bottom = r.pos.y - r.size.y * 0.5;
		outRect.right = r.pos.x + r.size.x * 0.5;
		outRect.left = r.pos.x - r.size.x * 0.5;
}

static LONG_PTR CALLBACK defaultWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	Window* win = Window::stGetWindow((winHandle)hwnd);
	switch (uMsg)
	{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			// All painting occurs here, between BeginPaint and EndPaint.
			uint8_t br = 10; uint8_t bg = 10; uint8_t bb = 20;
			HBRUSH bgBrush = CreateSolidBrush((0 << 24) | (bb << 16) | (bg << 8) | br);
			FillRect(hdc, &ps.rcPaint, bgBrush);
			EndPaint(hwnd, &ps);
		}

		case WM_SIZE: {
			NWIN_CALL_CALL_BACK(win->resizeCallback, (winHandle)hwnd);
			return 0;
		};
		case WM_DESTROY: {
			Window::stInvalidate(hwnd);
			PostQuitMessage(0);
			return 0;
		}

			return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


bool Window::shouldLoop() {
	return _shouldLoop;
}

int Window::update() {
	//TODO::Add update rate
	while (PeekMessage((LPMSG)_msgBuff, 0, 0, 0, PM_NOREMOVE)) {
		_shouldLoop = GetMessage((LPMSG)_msgBuff, 0, 0, 0);
		TranslateMessage((LPMSG)_msgBuff);
		DispatchMessage((LPMSG)_msgBuff);
	}
	return 1;
}

Window* Window::stCreateWindow(WindowCrtInfo& crtInfo) {

	HINSTANCE moduleInstance;
	LPCTSTR nameID;
	WNDCLASS wc{};
	RECT  winRect{};
	HANDLE h;


	h = NULL;

	moduleInstance = GetModuleHandle(NULL);
	std::string tempN = (std::string("_NWin_") + std::to_string(GET_NEW_ID));
	nameID = tempN.c_str();
	wc.lpszClassName = nameID;
	wc.lpfnWndProc = crtInfo.customWindowProcPtr != nullptr ? 
					(win_proc_ptr)(crtInfo.customWindowProcPtr)
					: &defaultWinProc;
	
	wc.hInstance = moduleInstance;
	
	RegisterClass(&wc);
	
	getWinRect(crtInfo.metrics,winRect);
	
	WIN_CHECK(h = CreateWindowExA(crtInfo.exStyle, 
								  nameID, crtInfo.description, 
								  crtInfo.style, crtInfo.metrics.pos.x, crtInfo.metrics.pos.y, 
								  crtInfo.metrics.size.x, crtInfo.metrics.size.y, 0, 0, moduleInstance, 0));
	if (!h) return nullptr;
	//Register and initialize members
	Window::_windowsMap.emplace(h, Window());
	Window& win = Window::_windowsMap[h];
	win._handle = h;
	win._msgBuff = new MSG;
	win._id = _incID;




	//Set winapi parameters
	ShowWindow((HWND)h, SW_SHOWDEFAULT);
	SetLayeredWindowAttributes((HWND)h, RGB(255, 0, 0), 100, LWA_ALPHA);

	
	return &win;
}

Window* Window::stGetWindow(winHandle handle) {
	auto iter = Window::_windowsMap.find(handle);
	return (iter == Window::_windowsMap.end()) ? nullptr : &iter->second;
}

int Window::destroy() {
	delete _msgBuff;
	Window::_windowsMap.erase(_handle);
	return 1;
}

winHandle Window::_getHandle() {
	return _handle;
}


int Window::stDestroyWindow(winHandle handle) {
	return DestroyWindow((HWND)handle);
}

int  Window::stDestroyWindow(Window* window) {
	return Window::stDestroyWindow(window->_handle);
}

bool Window::stInvalidate(winHandle handle) {
	auto iter = Window::_windowsMap.find(handle);
	if (iter != Window::_windowsMap.end()) return 0;
	iter->second._shouldLoop = 0;
	return 1;
}

bool	Window::stInvalidate(Window* window) {
	if (window == nullptr) return 0;
	window->_shouldLoop = 0;
	return 1;
}



//bool stShouldUpdate(winHandle* handle);
//bool stShouldUpdate(Window* window);
}