#include "window.h"

typedef int VulkanResult;
namespace NWin {
	VulkanResult createSurface(Window* window, void* vkInstanceHandle, void* vkSurfaceOutput ,void* allocationCallback  = nullptr);
}