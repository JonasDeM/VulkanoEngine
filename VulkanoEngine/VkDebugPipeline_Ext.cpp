
#include "stdafx.h"
#include "VkDebugPipeline_Ext.h"
#include <stdexcept>
#include <vulkan/vulkan.h>
#include "FileUtils.h"
#include <vector>
#include <array>
#include "VertexStructs.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"
#include "VkSwapchainKHR_Ext.h"
#include "HoldingContentLoader.h"

std::vector<VkPipelineShaderStageCreateInfo> VkDebugPipeline_Ext::CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const
{
	VkPipelineShaderStageCreateInfo vertStageCreateInfo = {};
	vertStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageCreateInfo.module = *shaderModuleLoader.GetContent(L"./Shaders/debugVert.spv");
	vertStageCreateInfo.pName = "main"; //entry point
										// optional pSpecializationInfo ::  It allows you to specify values for shader constants. 
										//You can use a single shader module where its behavior can be configured at pipeline creation
										//by specifying different values for the constants used in it.

	VkPipelineShaderStageCreateInfo fragStageCreateInfo = {};
	fragStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageCreateInfo.module = *shaderModuleLoader.GetContent(L"./Shaders/debugFrag.spv");;
	fragStageCreateInfo.pName = "main"; //entry point

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertStageCreateInfo, fragStageCreateInfo };
	return shaderStages;
}

VkPipelineVertexInputStateCreateInfo VkDebugPipeline_Ext::CreateVertexInputStateCreateInfo() const
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &VertexPosCol::InputBindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)VertexPosCol::AttributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = VertexPosCol::AttributeDescriptions.data();
	return vertexInputInfo;
}

VkPipelineInputAssemblyStateCreateInfo VkDebugPipeline_Ext::CreateInputAssemblyStateCreateInfo() const
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

VkPipelineRasterizationStateCreateInfo VkDebugPipeline_Ext::CreateRasterizationStateCreateInfo() const
{
	auto info = VkBasicGeometryPipeline_Ext::CreateRasterizationStateCreateInfo();
	info.polygonMode = VK_POLYGON_MODE_LINE;
	return info;
}
