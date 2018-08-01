#pragma once
#include "stdafx.h"

#include "VkTextureImage_Ext.h"
#include "VkPhysicalDevice_Ext.h"
#include <stdexcept>
#include "HandleUtilities.h"
#include "VulkanUtils.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"
#include "FileUtils.h"

VkTextureImage_Ext::VkTextureImage_Ext(VulkanContext* pVkContext, wstring filename) :
	VkImage_Ext(pVkContext, GetImageFileExtent(filename), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
	int texWidth, texHeight, texChannels;
	// as i try to do everywhere in this project, I want to make it so if an object is created i also make sure it gets cleaned up without having to worry about it somewhere else
	auto pixels = unique_ptr_del<stbi_uc>(stbi_load(FileUtils::ToString(filename).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha), stbi_image_free);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}
	VkDevice device = *pVkContext->GetVkDevice();

	auto stagingBuffer = CreateHandle<VkBuffer>(vkDestroyBuffer, device);
	auto stagingBufferMemory = CreateHandle<VkDeviceMemory>(vkFreeMemory, device);
	VulkanUtils::CreateBuffer(pVkContext, imageSize
		, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer.get(), stagingBufferMemory.get());

	void* data;
	vkMapMemory(device, *stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels.get(), static_cast<size_t>(imageSize));
	vkUnmapMemory(device, *stagingBufferMemory);

	TransitionImageLayout(pVkContext, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	CopyBufferToImage(pVkContext, *stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	TransitionImageLayout(pVkContext, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkExtent2D VkTextureImage_Ext::GetImageFileExtent(wstring filename)
{
	int texWidth, texHeight, texChannels;

	stbi_info(FileUtils::ToString(filename).c_str(), &texWidth, &texHeight, &texChannels);
	return VkExtent2D{ static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight) };
}

void VkTextureImage_Ext::CopyBufferToImage(VulkanContext* pVkContext, VkBuffer buffer, uint32_t width, uint32_t height) {
	VkCommandBuffer cmdBuffer = VulkanUtils::BeginSingleTimeCommands(pVkContext);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		cmdBuffer,
		buffer,
		*this,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	VulkanUtils::EndSingleTimeCommands(pVkContext, cmdBuffer);
}
