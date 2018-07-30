#pragma once

#include <vulkan/vulkan.h>
#include "VkPhysicalDevice_Ext.h"
class VulkanContext;

//compile-time utility to get the class that the pointer is pointing to
#define GET_CLASS_FROM_PTR(t) std::remove_pointer<decltype(t)>::type 

namespace VulkanUtils
{
	void CreateBuffer(const VulkanContext* pVkContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* bufferPtr, VkDeviceMemory* bufferMemoryPtr);
	
	VkCommandBuffer BeginSingleTimeCommands(const VulkanContext* pVkContext);

	void EndSingleTimeCommands(const VulkanContext* pVkContext, VkCommandBuffer commandBuffer);

	void CopyBuffer(const VulkanContext* pVkContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	bool HasStencilComponent(VkFormat format);
};
