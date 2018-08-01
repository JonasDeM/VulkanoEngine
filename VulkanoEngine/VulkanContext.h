#pragma once

class VkInstance_Ext;
class VkDevice_Ext;
class VkPhysicalDevice_Ext;
class VkSwapchainKHR_Ext;
class VkSurface_Ext;
class GameSettings;

enum VkContextFlags : char {
	InvalidDrawCommandBuffers = 1 << 0
};


class VulkanContext
{
public:
	VulkanContext() = default;
	//disabling copy
	VulkanContext(const VulkanContext&) = delete;
	void operator=(VulkanContext const &x) = delete;

	void virtual SetFlags(VkContextFlags flags)	{ m_Flags = static_cast<VkContextFlags>(m_Flags | flags);}
	bool virtual HasFlags(VkContextFlags flags) { return (m_Flags & flags) == flags;}

	const virtual VkInstance_Ext* GetVkInstance() const = 0;
	const virtual VkDevice_Ext* GetVkDevice() const = 0;
	const virtual VkPhysicalDevice_Ext* GetVkPhysicalDevice() const = 0;
	const virtual VkSwapchainKHR_Ext* GetVkSwapChain() const = 0;
	const virtual VkCommandPool* GetVkGraphicsCommandPool() const = 0;
	const virtual VkCommandPool* GetVkGraphicsCommandPoolTransient() const = 0;
	const virtual VkQueue GetVkGraphicsQueue() const = 0;
	const virtual VkQueue GetPresentQueue() const = 0;
	const virtual VkSurface_Ext* GetSurface() const = 0;

	virtual int GetCurrentFrameIndex() const = 0;

protected:
	VkContextFlags m_Flags = static_cast<VkContextFlags>(0);
};
