#pragma once
#include <vector>
#include "DeriveableHandle.h"
#include <memory>
#include <vulkan/vulkan.h>
#include "Surface.h"

struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentFamily = -1;

	bool isComplete() const {
		return graphicsFamily >= 0 && graphicsFamily >= 0;
	}
};

//Wrapper around VkPhysicalDevice, so i can call utility functions (special case because i don't have ownership)
class VkPhysicalDevice_Ext : public DeriveableHandle<VkPhysicalDevice>
{
public:
	// Physical devices are retrieved from the vulkan instance
	VkPhysicalDevice_Ext(VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface, std::vector<const char*> deviceExtensions) : DeriveableHandle<VkPhysicalDevice>(physicalDevice)
	{
		m_QueueFamilyIndices = std::make_unique<QueueFamilyIndices>();
		m_SwapChainSupportDetails = std::make_unique<SwapChainSupportDetails>();
		ControlAndRecordFeatures(surface, deviceExtensions);
	}

	bool IsSuitable() const { return m_IsSuitable; }
	const QueueFamilyIndices* GetQueueFamilyIndices() const { return m_QueueFamilyIndices.get(); }
	const SwapChainSupportDetails* GetSwapChainSupportDetails(bool refresh = false, VkSurfaceKHR surfaceForRefresh = VK_NULL_HANDLE) const
	{
		if (refresh)
			QuerySwapChainSupport(surfaceForRefresh);

		return m_SwapChainSupportDetails.get();
	}

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
private:
	//change this to your application's need
	void ControlAndRecordFeatures(const VkSurfaceKHR surface, std::vector<const char*> deviceExtensions);

	void QuerySwapChainSupport(const VkSurfaceKHR surface) const;
	bool CheckDeviceExtensionSupport(std::vector<const char*> deviceExtensions) const;
	void GetQueueFamilyIndices(const VkSurfaceKHR surface) const;

	bool m_IsSuitable = true;
	std::unique_ptr<QueueFamilyIndices> m_QueueFamilyIndices;
	std::unique_ptr<SwapChainSupportDetails> m_SwapChainSupportDetails;
};



