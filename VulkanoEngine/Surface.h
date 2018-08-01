#pragma once
#include <GLFW/glfw3.h>
#include "DeriveableHandle.h"

class VkSurface_Ext : public DeriveableHandle<VkSurfaceKHR>
{
public:
	VkSurface_Ext(VkInstance instance, GLFWwindow* window)
	{
		glfwCreateWindowSurface(instance, window, nullptr, this->BasePtr());
	}

	void Destroy(VkInstance instance) const { vkDestroySurfaceKHR(instance,*this, nullptr); }

private:


};

