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

struct CrtInfoWindowsExtra {
    uint32_t style;
    uint32_t exStyle;
};

uint32_t defaultWindowsStyle = (0x00000000L | 0x00C00000L | 0x00080000L | 0x00040000L | 0x00020000L | 0x00010000L);
uint32_t defaultWindowsExStyle = 0x00040000L;

Rect defaultWindowMetrics = { 100,100,480,360 };

std::unordered_map<winHandle, NWindow> windowsMap;

NWindow* GetWin(winHandle handle) {
    auto iter = windowsMap.find(handle);
    if (iter == windowsMap.end()) return 0;
    return &iter->second;
}

//-------------------------------------
int NWindow::update() {
    #ifdef NW_PLATFORM_WINDOWS
    LPMSG msg = (LPMSG)_data.win._msgBuff;
	while (PeekMessage(msg, 0, 0, 0, PM_NOREMOVE)) {
		_shouldLoop = GetMessage(msg, 0, 0, 0);
		TranslateMessage(msg);
		DispatchMessage(msg);
	}
	NWIN_CALL_CALL_BACK(_drawCallback, (winHandle)_handle)
	return 1;
    #endif
    return 0;
}

int NWindow::swapBuffers() {
    #ifdef NW_PLATFORM_WINDOWS
	SwapBuffers((HDC)_data.win._dcHandle);
	return 1;
    #endif
	return 0;
}

void SetWinProcCallback(NWindow* w,void* p) {
    #ifdef NW_PLATFORM_WINDOWS
    SetWindowLongPtr((HWND)(w->_handle), GWLP_WNDPROC, (LONG_PTR)p);
    #endif
}

int NWindow::clean() {
    #ifdef NW_PLATFORM_WINDOWS
	delete ((MSG*)_data.win._msgBuff);
	_keyboard.destroy();
    #endif
    return 1;
}


void NWindow::getMousePosition(Vec2& pos) {
    #ifdef NW_PLATFORM_WINDOWS
	POINT p;
	GetCursorPos(&p);
	ScreenToClient((HWND)_handle, &p);
	pos.x = p.x;
	pos.y = p.y;
    #endif
}

void NWindow::getDrawAreaSize(Vec2& size) {
    #ifdef NW_PLATFORM_WINDOWS
	RECT rec{};
	GetClientRect((HWND)_handle, &rec);
	size.x = rec.right - rec.left;
	size.y = rec.bottom - rec.top;
    #endif
}

void NWindow::disableTitleBar() {
    #ifdef NW_PLATFORM_WINDOWS
	DWORD style = GetWindowLong((HWND)_handle, GWL_STYLE);
	style &= ~_data.win._style;
	WIN_CHECK(SetWindowLong((HWND)_handle, GWL_STYLE, style));
    #endif
}

void NWindow::enableTitleBar() {
    #ifdef NW_PLATFORM_WINDOWS
	DWORD style = GetWindowLong((HWND)_handle, GWL_STYLE);
	style |= _data.win._style;
	WIN_CHECK(SetWindowLong((HWND)_handle, GWL_STYLE, style));
    #endif
}

void NWindow::setTitle(const char* newTitle) {
    #ifdef NW_PLATFORM_WINDOWS
	SetWindowText((HWND)_handle,newTitle);
    #endif
}
void NWindow::getTitle(char* title, int buffLen) {
    #ifdef NW_PLATFORM_WINDOWS
	GetWindowText((HWND)_handle, title, buffLen);
    #endif
}

#ifdef NW_PLATFORM_WINDOWS
static BOOL CALLBACK getMonitorCallback(
	HMONITOR monitorHandle,
	HDC	   hdc,
	LPRECT lpRect,
	LPARAM dwData
)
{
	std::vector<HMONITOR>* temp = ((std::vector<HMONITOR>*)dwData);
	temp->push_back(monitorHandle);
	return 1;
};

void getMonitor(HDC dcHandle, std::vector<HMONITOR>& outVector) {
	//"Success" is when all monitors are are enumerated, if gerMonitorCallback returns 0, 
	//then EnumDisplayMonitors returns 0
	//https://github.com/MicrosoftDocs/feedback/issues/1011
	WIN_CHECK(EnumDisplayMonitors(dcHandle, NULL, &getMonitorCallback, (LPARAM)&outVector));
}
#endif

void NWindow::enableFullScreen() {
    #ifdef NW_PLATFORM_WINDOWS
	std::vector<HMONITOR> outVec;
	getMonitor((HDC)_data.win._dcHandle, outVec);
	if (outVec.size() <= 0) return;
	MONITORINFO mInfo;
	mInfo.cbSize = sizeof(mInfo);
	WIN_CHECK(GetMonitorInfo(outVec[0], &mInfo));
	Vec2 size = { mInfo.rcMonitor.right - mInfo.rcMonitor.left, mInfo.rcMonitor.bottom - mInfo.rcMonitor.top };
	WIN_CHECK(SetWindowPos((HWND)_handle, NULL, 0, 0, size.x, size.y, 0));
	disableTitleBar();
    #endif
}

void NWindow::disableFullscreen(Rect& newMetrics) {
    #ifdef NW_PLATFORM_WINDOWS
	enableTitleBar();
	WIN_CHECK(SetWindowPos((HWND)_handle, NULL, newMetrics.pos.x, newMetrics.pos.y, newMetrics.size.x, newMetrics.size.y, 0));
    #endif
}

//-------------------------------------



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
