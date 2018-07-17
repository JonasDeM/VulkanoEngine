#pragma once
#include "DeriveableHandle.h"
#include "vulkan/vulkan.h"
#include "VkTextureImage_Ext.h"
//remove this class

class VkImageView_Ext :	public DeriveableHandle<VkImageView>
{
public:
	VkImageView_Ext(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void Destroy(VkDevice device) const
	{
		vkDestroyImageView(device, *this, nullptr);
	}
};

