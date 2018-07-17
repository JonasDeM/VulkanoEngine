#pragma once
#include "VkImage_Ext.h"
#include "HandleUtilities.h"

class VkPhysicalDevice_Ext;
class VulkanContext;

class VkDepthImage_Ext : public VkImage_Ext
{
public:
	VkDepthImage_Ext(VulkanContext* pVkContext);
	VkDepthImage_Ext::VkDepthImage_Ext(VulkanContext* pVkContext, VkExtent2D extent);

	static VkFormat GetDepthFormat(const VkPhysicalDevice_Ext* physicalDevice);
private:
	static VkFormat DEPTH_FORMAT;
};