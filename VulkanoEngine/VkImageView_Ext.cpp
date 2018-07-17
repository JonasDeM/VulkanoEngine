
#include "stdafx.h"

#include "VkImageView_Ext.h"
#include <stdexcept>
#include "VulkanUtils.h"


VkImageView_Ext::VkImageView_Ext(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	//The components field allows you to swizzle the color channels around.
	//For example you can map all of the channels to the red channel for a monochrome texture.
	//You can also map constant values of 0 and 1 to a channel.In our case we'll stick to the default mapping.


	if (vkCreateImageView(device, &viewInfo, nullptr, this->BasePtr()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}
}
