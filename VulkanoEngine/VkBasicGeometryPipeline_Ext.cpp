#pragma once
#include "stdafx.h"
#include "VkBasicGeometryPipeline_Ext.h"
#include "FileUtils.h"
#include "VertexStructs.h"
#include "HoldingContentLoader.h"

std::vector<VkPipelineShaderStageCreateInfo> VkBasicGeometryPipeline_Ext::CreateShaderStageCreateInfos(HoldingContentLoader<VkShaderModule>& shaderModuleLoader) const
{
	VkPipelineShaderStageCreateInfo vertStageCreateInfo = {};
	vertStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageCreateInfo.module = *shaderModuleLoader.GetContent(L"./Shaders/poscolnormVert.spv");
	vertStageCreateInfo.pName = "main"; //entry point
	// optional pSpecializationInfo ::  It allows you to specify values for shader constants. 
	//You can use a single shader module where its behavior can be configured at pipeline creation
	//by specifying different values for the constants used in it.

	VkPipelineShaderStageCreateInfo fragStageCreateInfo = {};
	fragStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageCreateInfo.module = *shaderModuleLoader.GetContent(L"./Shaders/poscolnormFrag.spv");;
	fragStageCreateInfo.pName = "main"; //entry point

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertStageCreateInfo, fragStageCreateInfo };
	return shaderStages;
}

VkPipelineVertexInputStateCreateInfo VkBasicGeometryPipeline_Ext::CreateVertexInputStateCreateInfo() const
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	//Never hardcode the vertextype, use the public typedef VertexType of the class
	vertexInputInfo.pVertexBindingDescriptions = &VertexType::InputBindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)VertexType::AttributeDescriptions.size();
	vertexInputInfo.pVertexAttributeDescriptions = VertexType::AttributeDescriptions.data();

	return vertexInputInfo;
}

VkDescriptorSetLayout VkBasicGeometryPipeline_Ext::CreateDescriptorSetLayout(VkDevice device) const
{
	//uniform variable like the marices to transform to screen space
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkDescriptorSetLayout descLayout;
	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	return descLayout;
}

unique_ptr_del<VkDescriptorPool> VkBasicGeometryPipeline_Ext::CreateDescriptorPool(VkDevice device, const int uboCount) const
{
	std::array<VkDescriptorPoolSize, 1> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = uboCount;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = uboCount;

	auto pVkDescriptorPool = CreateHandle<VkDescriptorPool>(vkDestroyDescriptorPool, device);
	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, pVkDescriptorPool.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
	return std::move(pVkDescriptorPool);
}

std::vector<VkDescriptorSet> VkBasicGeometryPipeline_Ext::CreateAndWriteDescriptorSets(VkDevice device, VkDescriptorPool descPool, const vector<unique_ptr_del<VkBuffer>>& uniformBuffers) const
{
	vector<VkDescriptorSet> descSets(uniformBuffers.size());

	std::vector<VkDescriptorSetLayout> layouts(uniformBuffers.size(), GetDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descPool;
	allocInfo.descriptorSetCount = (uint32_t)layouts.size();
	allocInfo.pSetLayouts = layouts.data();

	if (vkAllocateDescriptorSets(device, &allocInfo, descSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	std::vector<VkWriteDescriptorSet> descriptorWrites(uniformBuffers.size());
	for (size_t i = 0; i < uniformBuffers.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = *uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[i].dstSet = descSets[i];
		descriptorWrites[i].dstBinding = 0;
		descriptorWrites[i].dstArrayElement = 0;
		descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[i].descriptorCount = 1;
		descriptorWrites[i].pBufferInfo = &bufferInfo;
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	return descSets;
}

