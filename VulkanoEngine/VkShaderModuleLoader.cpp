#pragma once
#include "stdafx.h"
#include "VkShaderModuleLoader.h"
#include "FileUtils.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"

std::shared_ptr<VkShaderModule> VkShaderModuleLoader::LoadContent(const wstring & fileName) const
{
	auto code = FileUtils::ReadFile(fileName);
	VkDevice device = *m_pVulkanContext->GetVkDevice();
	//need to be cleaned up at the end
	auto shaderModule = CreateSharedHandle<VkShaderModule>(vkDestroyShaderModule, device);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size(); //codeSize is the size, in bytes, of the code pointed to by pCode, char == 1 byte defined by c++ standard
	createInfo.pCode = reinterpret_cast<uint32_t const*>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, shaderModule.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;
}