#pragma once
#include "HoldingContentLoader.h"

class VkShaderModuleLoader final: public HoldingContentLoader<VkShaderModule>
{
public:
	VkShaderModuleLoader(VulkanContext * pVkContext) : m_pVulkanContext(pVkContext) {};
	std::shared_ptr<VkShaderModule> LoadContent(const wstring& fileName) const override;
private:
	VulkanContext * m_pVulkanContext;
};
