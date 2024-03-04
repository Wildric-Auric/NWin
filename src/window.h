#pragma once
#include <cinttypes>
#include "util.h"
#include <unordered_map>


namespace NWin {

#define GET_NEW_ID (Window::_incID++)
#define NWIN_CALL_CALL_BACK(func, ...) if ((func) != nullptr) { func(__VA_ARGS__); }

typedef void* winHandle;
typedef void* MsgBuffer;
typedef void(*sigResizeCallback)(winHandle);



enum class WindowStyle : Word {
	Default = (0x00000000L | 0x00C00000L | 0x00080000L | 0x00040000L | 0x00020000L | 0x00010000L)
};

enum class WindowExStyle {
	Default = 0x02000000L
};



struct WindowCrtInfo {
	char* description         = nullptr;
	Rect  metrics{};
	void* customWindowProcPtr = nullptr;
	Word   style	  = (Word)WindowStyle::Default;
	Word   exStyle    = (Word)WindowExStyle::Default;
};

class Window {
private:
	uint64_t  _id      = 0;
	winHandle _handle  = nullptr;
	MsgBuffer _msgBuff = nullptr;
	bool _shouldLoop   = 1;

	static std::unordered_map<winHandle, Window> _windowsMap;
	static uint64_t _incID;

public:
	Window()						 = default;
	Window(Window&& other)			 = default;
	Window(const Window&)		     = delete;
	Window& operator=(const Window&) = delete;

	winHandle _getHandle();
	int  update();
	int	 destroy();
	bool shouldLoop();

	//Callback function----------
	sigResizeCallback resizeCallback = nullptr;
	//Static---------------------
	static Window*  stGetWindow(winHandle handle);
	static Window*	stCreateWindow(WindowCrtInfo& crtInfo);
	static int		stDestroyWindow(winHandle handle);
	static int      stDestroyWindow(Window* window);
	static bool		Window::stInvalidate(winHandle handle);
	static bool		Window::stInvalidate(Window* window);


	/*static bool	stShouldUpdate(Window* window);
	static bool		stShouldUpdate(winHandle* handle);*/

	};

}



