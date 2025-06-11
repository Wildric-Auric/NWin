#include "util.h"
#include "keyboard.h"

namespace NWin {

extern Rect defaultWindowMetrics;
extern uint32_t defaultWindowsStyle;
extern uint32_t defaultWindowsExStyle;

typedef void* winHnd;
typedef void(*procResizeCallback)(winHnd, Vec2 newSize);
typedef void(*procDrawCallback)(winHnd);


typedef void* winHandle;
typedef void* MsgBuffer;
typedef void* deviceContextHandle;
typedef	void* applicationInstance;
struct WindowsData {	
	deviceContextHandle _dcHandle	= nullptr;
	applicationInstance _instance   = nullptr;
	MsgBuffer           _msgBuff	= nullptr;
    uint32_t            _style      = 0;
};

typedef void* lxfbConfig;
typedef void* lxdisp;
struct LinuxData {
   lxfbConfig fbConf; 
   lxdisp     display;
};

struct PlatformData {
    union {
        WindowsData win;
        LinuxData   lx;
    };
};

class NWindow {
    public:
    NWindow(); 
	NWindow(NWindow&& other)			 = default;
	NWindow(const NWindow&)		     = delete;
	NWindow& operator=(const NWindow&) = delete;

    void getDrawAreaSize(const Vec2&);
    void getMousePosition(const Vec2&);
    int update();
    int swapBuffers();
    int clean();

    void disableTitleBar();
    void enableTitleBar();
    void setTitle(const char*);
    void getTitle(char*, int cap);
    void enableFullScreen();
    void disableFullscreen(Rect& = defaultWindowMetrics);

    inline void      setResizeCallback(procResizeCallback proc) {_resizeCallback = proc;}
    inline void      setDrawCallback(procDrawCallback proc) {_drawCallback = proc;}
    inline Keyboard& getKeyboard() {return _keyboard;};

    uint64_t  _id          = 0;
    winHandle _handle	   = nullptr;
    bool               _shouldLoop      = 1;
	procResizeCallback _resizeCallback  = nullptr;
	procDrawCallback   _drawCallback	= nullptr;
    Keyboard _keyboard;
    PlatformData _data; 
};

struct WinCrtInfo {
    char* desc = nullptr;
    Rect  metrics         = defaultWindowMetrics;
    bool  drawAreaMetrics = true;
    int   inputBufferSize   = 256;
    void* customWindowProc= nullptr;
    char* description     = nullptr;
    void* next;
};

NWindow*    GetWin(winHandle handle);
NWindow*	CreateWin(WinCrtInfo& crtInfo);
int         DestroyWin(NWindow* window);
void		ShouldNotUpdate(NWindow*    window);

#ifdef _WINDOWS_
bool dwmBlur(NWindow*);
bool dwmDarkModeFrame(NWindow*,bool flag);
bool dwmDontRoundCorners(NWindow*,bool flag);
#endif
}
