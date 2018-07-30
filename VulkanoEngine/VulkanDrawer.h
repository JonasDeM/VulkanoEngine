#pragma once
#include "VulkanContext.h"
#include <future>

class VulkanDrawer : public VulkanContext
{
public:
	VulkanDrawer(GLFWwindow* targetWindow);

	//getters
	const VkInstance_Ext* GetVkInstance() const override { return m_pVkInstance.get(); }
	const VkDevice_Ext* GetVkDevice() const override { return m_pVkDevice.get(); }
	const VkPhysicalDevice_Ext* GetVkPhysicalDevice() const override { return m_pVkPhysicalDevice.get(); }
	const VkSwapchainKHR_Ext* GetVkSwapChain() const override { return m_pVkSwapchain.get(); }
	const VkCommandPool* GetVkGraphicsCommandPool() const override { return m_pVkGraphicsCommandPool.get(); } //only use 2 now, should probably use even more different types for different tasks
	const VkCommandPool* GetVkGraphicsCommandPoolTransient() const override { return m_pVkGraphicsCommandPoolTransient.get(); } // For commands with short life-span
	const VkQueue GetVkGraphicsQueue() const override { return m_VkGraphicsQueue; }
	const VkQueue GetPresentQueue() const override { return m_VkPresentQueue; }
	const VkSurface_Ext* GetSurface() const override { return m_pVkSurface.get(); }

	int GetCurrentDrawingBufferIndex() const override { return m_FrameIndexToUpdate; }
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
	void CreateCommandPools();
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
	unique_ptr_del<VkCommandPool> m_pVkGraphicsCommandPool, m_pVkGraphicsCommandPoolTransient;
	unique_ptr_del<VkSemaphore> m_pVkImageAvailableSemaphore;
	unique_ptr_del<VkSemaphore> m_pVkRenderFinishedSemaphore;
	unique_ptr_del<VkDescriptorPool> m_pVkDescriptorPool;

	int m_FrameIndexToUpdate = 0;

	//Async
	std::future<VkResult> m_PresentImageFuture;
};


