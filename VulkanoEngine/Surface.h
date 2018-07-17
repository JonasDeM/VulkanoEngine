#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <memory>
#include "DeriveableHandle.h"

//class VkPhysicalDevice_Ext;

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


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

