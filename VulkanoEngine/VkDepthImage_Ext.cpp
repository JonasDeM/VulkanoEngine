#pragma once
#include "stdafx.h"

#include "VkDepthImage_Ext.h"
#include "VkPhysicalDevice_Ext.h"
#include <stdexcept>
#include "HandleUtilities.h"
#include "VulkanUtils.h"
#include "VulkanContext.h"
#include "VkSwapchainKHR_Ext.h"


VkFormat VkDepthImage_Ext::DEPTH_FORMAT = VK_FORMAT_UNDEFINED;

VkDepthImage_Ext::VkDepthImage_Ext(VulkanContext* pVkContext) :
	VkImage_Ext(pVkContext, pVkContext->GetVkSwapChain()->GetExtent(), GetDepthFormat(pVkContext->GetVkPhysicalDevice()), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
}

//constructor for use when the swapchain isn't created yet
VkDepthImage_Ext::VkDepthImage_Ext(VulkanContext* pVkContext, VkExtent2D extent) :
	VkImage_Ext(pVkContext, extent, GetDepthFormat(pVkContext->GetVkPhysicalDevice()), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
{
}

VkFormat VkDepthImage_Ext::GetDepthFormat(const VkPhysicalDevice_Ext* physicalDevice)
{
	if (DEPTH_FORMAT == VK_FORMAT_UNDEFINED)
	{
		DEPTH_FORMAT = physicalDevice->FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	return DEPTH_FORMAT;
}

