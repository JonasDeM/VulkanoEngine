#pragma once
#include "VulkanContext.h"

class VulkanDrawer : public VulkanContext
{
public:
	VulkanDrawer(GLFWwindow* targetWindow);

	//getters
	const VkInstance_Ext* GetVkInstance() const { return m_pVkInstance.get(); }
	const VkDevice_Ext* GetVkDevice() const { return m_pVkDevice.get(); }
	const VkPhysicalDevice_Ext* GetVkPhysicalDevice() const { return m_pVkPhysicalDevice.get(); }
	const VkSwapchainKHR_Ext* GetVkSwapChain() const { return m_pVkSwapchain.get(); }
	const VkCommandPool* GetVkCommandPool() const { return m_pVkCommandPool.get(); } //now  i use 1 general commandpool, should probably use different types for different tasks
	const VkQueue GetVkGraphicsQueue() const { return m_VkGraphicsQueue; }
	const VkQueue GetPresentQueue() const { return m_VkPresentQueue; }
	const VkSurface_Ext* GetSurface() const { return m_pVkSurface.get(); }
	//const VkDescriptorPool* GetDescriptorPool() { return m_pVkDescriptorPool.get(); }

	void VkDrawFrame(GameSettings* settings);

private:
	void ClearFlags(VkContextFlags flags);

	bool CheckValidationLayerSupport();
	void PickPhysicalDevice();
	//extension choices
	std::vector<const char*> ChooseVulkanExtensions() const;
	//Validation Layers
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj,
		size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);
	void SetupDebugCallback();
	void CreateCommandPool();
	void CreateSemaphores();
	void CreateDescriptorPool();
	void RecreateVkSwapChain(GameSettings* settings);
	void CreateDrawCommandBuffers(GameSettings* settings);

	const std::vector<const char*> m_ValidationLayers = {
		"VK_LAYER_LUNARG_standard_validation" // is a 'group' of all standard usefull validation layers
	};
#ifdef NDEBUG
	const bool m_EnableValidationLayers = false;
#else
	const bool m_EnableValidationLayers = true;
#endif
	const std::vector<const char*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	unique_ptr_del<VkInstance_Ext> m_pVkInstance;
	unique_ptr_del<VkDevice_Ext> m_pVkDevice;
	std::unique_ptr<VkPhysicalDevice_Ext> m_pVkPhysicalDevice; //is implicitly destroyed when VkInstance is destroyed -> no need for custom del, its destructor is sufficient
	unique_ptr_del<VkSurface_Ext> m_pVkSurface;
	unique_ptr_del<VkSwapchainKHR_Ext> m_pVkSwapchain;
	std::vector<VkCommandBuffer> m_DrawCommandBuffers;

	unique_ptr_del<VkDebugReportCallbackEXT> m_pVkCallback;
	VkQueue m_VkGraphicsQueue; //Queue handles get cleaned up implicitly when the VkDevice is destroyed
	VkQueue m_VkPresentQueue;
	unique_ptr_del<VkCommandPool> m_pVkCommandPool;
	unique_ptr_del<VkSemaphore> m_pVkImageAvailableSemaphore;
	unique_ptr_del<VkSemaphore> m_pVkRenderFinishedSemaphore;
	unique_ptr_del<VkDescriptorPool> m_pVkDescriptorPool;
};


