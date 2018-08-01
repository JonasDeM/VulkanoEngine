#pragma once
#include "DeriveableHandle.h"
#include "VkTextureImage_Ext.h"

class VkImageView_Ext :	public DeriveableHandle<VkImageView>
{
public:
	VkImageView_Ext(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void Destroy(VkDevice device) const
	{
		vkDestroyImageView(device, *this, nullptr);
	}
};

