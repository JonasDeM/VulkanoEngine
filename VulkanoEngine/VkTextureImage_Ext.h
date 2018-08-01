#pragma once
#include "VkImage_Ext.h"
class VkPhysicalDevice_Ext;

class VkTextureImage_Ext : public VkImage_Ext
{
public:
	VkTextureImage_Ext(VulkanContext* pVkContext, wstring filename);

private:
	VkExtent2D GetImageFileExtent(wstring filename);
	void CopyBufferToImage(VulkanContext* pVkContext, VkBuffer buffer, uint32_t width, uint32_t height);

};