#include "nwindow.h"
#include <unordered_map>
#include <string>

#define NW_PLATFORM_WINDOWS
#ifdef NW_PLATFORM_WINDOWS
#include <Windows.h>
#include <winuser.h>
#include <winres.h>
#include <dwmapi.h>
typedef LRESULT  (CALLBACK* win_proc_ptr)(HWND, UINT, WPARAM, LPARAM);
#endif

#define NWIN_CALL_CALL_BACK(func, ...) if ((func) != nullptr) { func(__VA_ARGS__); }

namespace NWin {
#ifdef NW_PLATFORM_WINDOWS
LRESULT CALLBACK defaultWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	KeyEvent event;
	NWindow* win = GetWin((winHandle)hwnd);
	switch (uMsg)
	{
		//Inputs------------
		case WM_KEYDOWN:
			event.key		= (Key)wParam;
			event.eventType = NWIN_KeyPressed;
			win->getKeyboard().record(event);
			break;

		case WM_KEYUP:
			event.key = (Key)wParam;
			event.eventType = NWIN_KeyReleased;
			win->getKeyboard().record(event);
			break;

		case WM_LBUTTONDOWN:
			event.key = NWIN_KEY_LBUTTON;
			event.eventType = NWIN_KeyPressed;
			win->getKeyboard().record(event);
			break;

		case WM_LBUTTONUP:
			event.key = NWIN_KEY_LBUTTON;
			event.eventType = NWIN_KeyReleased;
			win->getKeyboard().record(event);
			break;

		case WM_RBUTTONDOWN:
			event.key = NWIN_KEY_RBUTTON;
			event.eventType = NWIN_KeyPressed;
			win->getKeyboard().record(event);
			break;

		case WM_RBUTTONUP:
			event.key = NWIN_KEY_RBUTTON;
			event.eventType = NWIN_KeyReleased;
			win->getKeyboard().record(event);
			break;
		//-------------------

		case WM_CREATE: 
		{
			RECT rcClient;
			GetWindowRect(hwnd, &rcClient);  
			SetWindowPos(hwnd, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom -rcClient.top, SWP_FRAMECHANGED);
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			if (win != nullptr)
				ReleaseDC((HWND)win->_handle, hdc);
			return 0;
		}
		case WM_SIZE: {
			if (win == nullptr) return 0;
			NWIN_CALL_CALL_BACK(win->_resizeCallback, (winHandle)hwnd, {LOWORD(lParam), HIWORD(lParam)});
			NWIN_CALL_CALL_BACK(win->_drawCallback, (winHandle)hwnd);
			return 0;
		};
		case WM_DESTROY: {
			win->_shouldLoop = 0;
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif

uint32_t _incID = 1;
#define GET_NEW_ID (_incID++)

uint32_t defaultWindowsStyle = (0x00000000L | 0x00C00000L | 0x00080000L | 0x00040000L | 0x00020000L | 0x00010000L);
uint32_t defaultWindowsExStyle = 0x00040000L;

Rect defaultWindowMetrics = { 100,100,480,360 };


std::unordered_map<winHandle, NWindow> windowsMap;


NWindow* GetWin(winHandle handle) {
    auto iter = windowsMap.find(handle);
    if (iter == windowsMap.end()) return 0;
    return &iter->second;
}

struct CrtInfoWindowsExtra {
    uint32_t style;
    uint32_t exStyle;
};

NWindow* PltWinCreateWin(WinCrtInfo& crtInfo) {
	NWindow* win = 0; 
#ifdef NW_PLATFORM_WINDOWS
    Word style   = defaultWindowsStyle;
    Word exStyle = defaultWindowsExStyle;
    if (crtInfo.next) 
        style = ((CrtInfoWindowsExtra*)crtInfo.next)->style;
    if (crtInfo.next) 
        exStyle = ((CrtInfoWindowsExtra*)crtInfo.next)->exStyle;
	HINSTANCE moduleInstance;
	LPCTSTR nameID;
	WNDCLASS wc{};
	RECT  winRect{};
	HANDLE h = 0;
	moduleInstance = GetModuleHandle(0);
	std::string tempN = (std::string("_NWin_") + std::to_string(GET_NEW_ID));
	nameID = tempN.c_str();
	wc.lpszClassName = nameID;
	wc.lpfnWndProc   = crtInfo.customWindowProc != nullptr ? 
    (win_proc_ptr)(crtInfo.customWindowProc) : &defaultWinProc;
	wc.hInstance	 = moduleInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor	     = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);
	winRect.left   = crtInfo.metrics.pos.x;
	winRect.top    = crtInfo.metrics.pos.y;
	winRect.bottom = crtInfo.metrics.pos.y + crtInfo.metrics.size.y;
	winRect.right  = crtInfo.metrics.pos.x + crtInfo.metrics.size.x;
    if (crtInfo.drawAreaMetrics) {
		AdjustWindowRect(&winRect, style, false);
	}
	WIN_CHECK(h = CreateWindowExA(exStyle,
		nameID, crtInfo.description,
		style, winRect.left, winRect.top,
		winRect.right - winRect.left, winRect.bottom - winRect.top, 0, 0, moduleInstance, 0));
	if (!h) return nullptr;
	//Register and initialize members
	windowsMap.emplace(h, NWindow());
	win = &windowsMap[h];
	win->_data.win._instance =  moduleInstance;
	win->_handle = h;
	win->_data.win._msgBuff = new MSG;
	win->_id = _incID;
	win->_data.win._dcHandle = GetDC((HWND)h);
	win->_data.win._style    = style;
	WIN_CHECK(win->_data.win._dcHandle);
	//Set winapi parameterss
	ShowWindow((HWND)h, SW_SHOWDEFAULT); //Returns false if the window isn't visible already; does not return error directly
	//WIN_CHECK(SetLayeredWindowAttributes((HWND)h, RGB(255, 0, 0), 100, LWA_ALPHA)); Only if WS_TRANSPARENT is set
	win->_keyboard.create(crtInfo.inputBufferSize);
#endif
    return win;
}

NWindow* PltLnxCreateWin(WinCrtInfo& crtInfo) {
    NWindow* win = 0;
#ifndef NW_PLATFORM_WINDOWS
    
#endif
    return win;
}


NWindow*    CreateWin(WinCrtInfo& crtInfo) {
	NWindow* win = 0; 
#ifdef NW_PLATFORM_WINDOWS
    win = PltWinCreateWin(crtInfo);
#else
    win = PltLnxCreateWin(crtInfo);
#endif
	return win;
};

int  DestroyWin(NWindow* window) {
    winHandle* h = (winHandle*)window->_handle;
    window->clean();
    int ret = 0;
#ifdef NW_PLATFORM_WINDOWS
    ret = DestroyWindow((HWND)h);
#endif

    windowsMap.erase(h);
    return ret;
}

void		ShouldNotUpdate(NWindow*    window) {
    auto it = windowsMap.find(window->_handle);
    if (it == windowsMap.end()) return;
    it->second._shouldLoop = 0;
}

}
