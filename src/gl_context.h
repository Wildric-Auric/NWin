#include "window.h"



namespace NWin {

typedef void* contextHandle;


enum class NWIN_GL_STATUS {
	NONE = 0,
	PIXEL_FORMAT_CHOICE_FAILURE,
	SET_PIXEL_FORMAT_FAILURE,
	CONTEXT_CREATION_FAILURE,
	CONTEXT_MAKE_CURRENT_FAILURE,
	CORE_OPENGL_INIT_FAILURE
};

class GlContext {
private:
	static GlContext* _current;
	
	Window*			_attachedWindow = nullptr;
	contextHandle	_contextHandle  = nullptr;
public:

	bool			isCurrent();
	NWIN_GL_STATUS	create(Window* window );
	NWIN_GL_STATUS  makeCurrent(bool noContext = 0);
	NWIN_GL_STATUS  initCoreOpenGL();
	Window*		    getWindow();

	static GlContext* getCurrent();
};

};