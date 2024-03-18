#pragma once
#include <cinttypes>
#include <unordered_map>
#include "util.h"


namespace NWin {

#define GET_NEW_ID (Window::_incID++)
#define NWIN_CALL_CALL_BACK(func, ...) if ((func) != nullptr) { func(__VA_ARGS__); }

typedef void* winHandle;
typedef void* MsgBuffer;
typedef void* deviceContextHandle;
typedef	void* applicationInstance;

typedef void(*procResizeCallback)(winHandle, Vec2 newSize);
typedef void(*procDrawCallback)(winHandle);


enum class WindowStyle : Word {
	Default = (0x00000000L | 0x00C00000L | 0x00080000L | 0x00040000L | 0x00020000L | 0x00010000L)
};

enum class WindowExStyle {
	Default = 0x00040000L //->WS_EX_APPWINDOW ; ----- 0x00000020L | 0x02000000L //Transparent | Composited
};


extern Rect defaultWindowMetrics;

struct WindowCrtInfo {
	char* description         = nullptr;
	Rect  metrics{};
	void* customWindowProcPtr = nullptr;
	Word  style				  = (Word)WindowStyle::Default;
	Word  exStyle			  = (Word)WindowExStyle::Default;
};

class Window {
private:
	uint64_t  _id					= 0;
	winHandle _handle				= nullptr;
	deviceContextHandle _dcHandle	= nullptr;
	applicationInstance  _instance  = nullptr;
	MsgBuffer _msgBuff			    = nullptr;
	bool _shouldLoop			    = 1;
	Word _style						= 0; //Mainly to enable and disable title bar

	static std::unordered_map<winHandle, Window> _windowsMap;
	static uint64_t _incID;

public:
	Window()						 = default;
	Window(Window&& other)			 = default;
	Window(const Window&)		     = delete;
	Window& operator=(const Window&) = delete;

	winHandle			_getHandle();
	deviceContextHandle _getDcHandle();
	applicationInstance _getInstance();

	Vec2 getDrawAreaSize();
	int  update();
	int  swapBuffers();
	int	 destroy();
	bool shouldLoop();
	//Styles setters
	void disableTitleBar();
	void enableTitleBar();

	void enableFullscreen();
	void disableFullscreen(Rect& newMetrics = defaultWindowMetrics);
	

	//Callback setters
	void setResizeCallback(procResizeCallback);
	void setDrawCallback(procDrawCallback);
	void setGdiDrawCallback(procDrawCallback);
	//DWM
	bool dwmBlur();					     //Not working; it should blur the background though
	bool dwmDarkModeFrame(bool flag);	 //if the flag is set on true, the window uses user's theme for its frame (non client area) color; otherwise, it is white.
	bool dwmDontRoundCorners(bool flag); //if the flag is set on true, window's corners are not rounded, otherwise the system chooses
	

	//Callback function----------
	procResizeCallback resizeCallback  = nullptr;
	procDrawCallback   drawCallback	  = nullptr;
	procDrawCallback   gdiDrawCallback = nullptr;
	//Static---------------------
	static Window*  stGetWindow(winHandle handle);
	static Window*	stCreateWindow(WindowCrtInfo& crtInfo);
	static int		stDestroyWindow(winHandle handle);
	static int      stDestroyWindow(Window* window);
	static bool		Window::stInvalidate(winHandle handle);
	static bool		Window::stInvalidate(Window* window);

	static Window* _stCreateRawWindow(WindowCrtInfo& crtInfo);
	/*static bool	stShouldUpdate(Window* window);
	static bool		stShouldUpdate(winHandle* handle);*/
	};

}



