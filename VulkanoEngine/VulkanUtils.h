#pragma once

#include <vulkan/vulkan.h>
#include "VkPhysicalDevice_Ext.h"
class VulkanContext;

#define GET_CLASS_FROM_PTR(t) std::remove_pointer<decltype(t)>::type //compile-time utility to get the class that the pointer is pointing to

namespace VulkanUtils
{
	void CreateBuffer(const VulkanContext* pVkContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* bufferPtr, VkDeviceMemory* bufferMemoryPtr);
	
	VkCommandBuffer BeginSingleTimeCommands(const VulkanContext* pVkContext);

	void EndSingleTimeCommands(const VulkanContext* pVkContext, VkCommandBuffer commandBuffer);

	// should actually use another command pool, specifically for short lived command buffers -> with the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
	void CopyBuffer(const VulkanContext* pVkContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	bool HasStencilComponent(VkFormat format);
	
};
