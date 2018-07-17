#pragma once
#include "DeriveableHandle.h"
#include "HandleUtilities.h"

class VkPhysicalDevice_Ext; 
class VulkanContext;

class VkImage_Ext :	public DeriveableHandle<VkImage>
{
public:
	VkImage_Ext(VulkanContext* pVkContext, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	void Destroy(VkDevice device) const
	{
		vkDestroyImage(device, *this, nullptr);
	}
	void TransitionImageLayout(VulkanContext* pVkContext, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

private:
	unique_ptr_del<VkDeviceMemory> m_ImageMemory;

};

