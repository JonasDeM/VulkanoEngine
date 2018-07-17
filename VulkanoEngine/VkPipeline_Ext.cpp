#include "stdafx.h"
#include "VkPipeline_Ext.h"

void VkPipeline_Ext::CreateShaderModule(const std::vector<char>& code, VkShaderModule & shaderModule, VkDevice device)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size(); //codeSize is the size, in bytes, of the code pointed to by pCode, char == 1 byte defined by c++ standard
	createInfo.pCode = reinterpret_cast<uint32_t const*>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
}