//TODO::Untested...

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"
#include "vulkan_surface.h"

//TODO::
#define VK_CHECK(expr) if ( (expr) == NULL) {return 0;}

static const char* RequiredVKExt[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };

namespace NWin {
	#define	VK_CREATE_SURFACE_PROC "vkCreateWin32SurfaceKHR"


	VulkanResult createSurface(Window* window, void* vkInstanceHandle, void* vkSurfaceOutput, void* allocationCallback) {
			VkResult res;
			VkWin32SurfaceCreateInfoKHR crtInfo{};
			//Load function, TODO::Make sure extensino are loaded (add an initialisation phase maybe?)
			PFN_vkCreateWin32SurfaceKHR createWin32Surface = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr((VkInstance)vkInstanceHandle, VK_CREATE_SURFACE_PROC);
			VK_CHECK(createWin32Surface);
			//Fill crtInfo
			crtInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			crtInfo.hinstance = (HINSTANCE)window->_getInstance();
			crtInfo.hwnd      = (HWND)window->_getHandle();
			crtInfo.pNext     = nullptr;
			crtInfo.flags     = NULL;
			res				  = createWin32Surface((VkInstance)vkInstanceHandle, 
                                &crtInfo, (const VkAllocationCallbacks*)allocationCallback, 
                                (VkSurfaceKHR*)vkSurfaceOutput);

			return res;
	}

    const char** getRequiredExt() {
        return RequiredVKExt;    
    }
}

