#pragma once
#include "VkContentLoader.h"
#include "VkTextureImage_Ext.h"
#include <memory>
#include <string>

class VkTextureLoader : public VkContentLoader<VkTextureImage_Ext>
{
public:
	VkTextureLoader() {};
	std::shared_ptr<VkTextureImage_Ext> LoadContent(const wstring& fileName, VulkanContext* vkContext) const override;
};
