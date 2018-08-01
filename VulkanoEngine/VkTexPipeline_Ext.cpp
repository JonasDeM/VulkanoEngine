#pragma once
#include "stdafx.h"
#include "VkTexPipeline_Ext.h"

unique_ptr_del<VkDescriptorPool> VkTexPipeline_Ext::CreateDescriptorPool(VkDevice device, const int uboCount) const
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = uboCount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = uboCount;

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

std::vector<VkDescriptorSet> VkTexPipeline_Ext::CreateAndWriteDescriptorSets(VkDevice device, VkDescriptorPool descPool, const vector<unique_ptr_del<VkBuffer>>& uniformBuffers, VkImageView texImageView, VkSampler texSampler) const
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

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texImageView;
	imageInfo.sampler = texSampler;

	std::vector<VkWriteDescriptorSet> descriptorWrites(uniformBuffers.size() * 2);
	for (size_t i = 0; i < uniformBuffers.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = *uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		auto dwIndex = i * 2;
		descriptorWrites[dwIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[dwIndex].dstSet = descSets[i];
		descriptorWrites[dwIndex].dstBinding = 0;
		descriptorWrites[dwIndex].dstArrayElement = 0;
		descriptorWrites[dwIndex].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[dwIndex].descriptorCount = 1;
		descriptorWrites[dwIndex].pBufferInfo = &bufferInfo;

		dwIndex++;
		descriptorWrites[dwIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[dwIndex].dstSet = descSets[i];
		descriptorWrites[dwIndex].dstBinding = 1;
		descriptorWrites[dwIndex].dstArrayElement = 0;
		descriptorWrites[dwIndex].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[dwIndex].descriptorCount = 1;
		descriptorWrites[dwIndex].pImageInfo = &imageInfo;
	}
	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	return descSets;
}

VkDescriptorSetLayout VkTexPipeline_Ext::CreateDescriptorSetLayout(VkDevice device) const
{
	//uniform variable like the marices to transform to screen space
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

												   //texture
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; //texture in fragment shader ofcourse

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

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