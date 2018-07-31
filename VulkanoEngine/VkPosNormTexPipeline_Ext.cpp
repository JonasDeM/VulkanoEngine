#pragma once

#include "stdafx.h"
#include "VkPosNormTexPipeline_Ext.h"

#include <stdexcept>
#include <vulkan/vulkan.h>
#include "FileUtils.h"
#include <vector>
#include <array>
#include "VertexStructs.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"
#include "HoldingContentLoader.h"

std::vector<VkPipelineShaderStageCreateInfo> VkPosNormTexPipeline_Ext::CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const
{
	VkPipelineShaderStageCreateInfo vertStageCreateInfo = {};
	vertStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageCreateInfo.module = *shaderModuleLoader.GetContent(L"./Shaders/posnormtexVert.spv");
	vertStageCreateInfo.pName = "main"; //entry point
										// optional pSpecializationInfo ::  It allows you to specify values for shader constants. 
										//You can use a single shader module where its behavior can be configured at pipeline creation
										//by specifying different values for the constants used in it.

	VkPipelineShaderStageCreateInfo fragStageCreateInfo = {};
	fragStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageCreateInfo.module = *shaderModuleLoader.GetContent(L"./Shaders/posnormtexFrag.spv");;
	fragStageCreateInfo.pName = "main"; //entry point

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertStageCreateInfo, fragStageCreateInfo };
	return shaderStages;
}

VkPipelineVertexInputStateCreateInfo VkPosNormTexPipeline_Ext::CreateVertexInputStateCreateInfo() const
{
	//Vertexinput
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	//Never hardcode the vertextype, use the public typedef VertexType of the class
	vertexInputInfo.pVertexBindingDescriptions = &VertexType::InputBindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)VertexType::AttributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = VertexType::AttributeDescriptions.data();

	return vertexInputInfo;
}

