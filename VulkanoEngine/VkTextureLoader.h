#pragma once
#include "LowMemoryContentLoader.h"
#include "VkTextureImage_Ext.h"

class VulkanContext;

class VkTextureLoader final: public LowMemoryContentLoader<VkTextureImage_Ext>
{
public:
	VkTextureLoader(VulkanContext* vkContext) : m_pVulkanContext(vkContext) {};
	std::shared_ptr<VkTextureImage_Ext> LoadContent(const wstring& fileName) const override;
private:
	VulkanContext * m_pVulkanContext;
};
