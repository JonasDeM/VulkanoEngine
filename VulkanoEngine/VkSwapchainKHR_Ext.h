#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "VkPosColTexPipeline_Ext.h"
#include <memory>
#include <algorithm>
#include "DeriveableHandle.h"
#include "VkPhysicalDevice_Ext.h"
#include "HandleUtilities.h"
#include "VertexStructs.h"
#include <chrono>
#include "VkBasicSampler_Ext.h"
#include "VkImageView_Ext.h"
#include "VkTextureImage_Ext.h"
#include "VkDepthImage_Ext.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h" // needed for implicit conversion

struct SwapChainSupportDetails;
struct QueueFamilyIndices;

class VkSwapchainKHR_Ext : public DeriveableHandle<VkSwapchainKHR>
{
public:

	VkSwapchainKHR_Ext(VulkanContext* pVkContext, const VkExtent2D& windowExtent, VkSwapchainKHR oldSwapChain)
	{
		CreateSwapChain(pVkContext, windowExtent, oldSwapChain);
		CreateImageViews(*pVkContext->GetVkDevice());
		CreateDepthResources(pVkContext);
		CreateRenderPass(pVkContext);
		CreateFrameBuffers(*pVkContext->GetVkDevice());
	}
	void Destroy(VkDevice device)
	{
		vkDestroySwapchainKHR(device, *this, nullptr);
	}
	
	int GetAmountImages() const { return (int)m_SwapChainImages.size(); }
	VkExtent2D GetExtent() const { return m_SwapChainExtent; }
	VkRenderPass GetRenderPass() const { return *m_RenderPass; }
	VkFramebuffer GetFrameBuffer(int i) const { return *m_SwapChainFramebuffers[i]; }
private:
	//swapchain choices
	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
				&&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		//we could rank formats and pick the best 

		return availableFormats[0];
	}
	static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		VkPresentModeKHR preferedMode = VK_PRESENT_MODE_FIFO_KHR;//guarenteed to be available

		for (const auto availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				preferedMode = availablePresentMode;
		}

		return preferedMode;
	}

	VkExtent2D GetActualSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, const VkExtent2D& windowExtent) const
	{
		if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return windowExtent;
		else
		{
			//int width, height;
			//glfwGetWindowSize(m_Window, &width, &height);
			VkExtent2D actualExtent = {
				std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, windowExtent.width))
				,std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, windowExtent.height))
			};
			return actualExtent;
		}
	}

	//what swapchain features the gpu supports..

	void CreateSwapChain(VulkanContext* pVkContext, const VkExtent2D& windowExtent, VkSwapchainKHR oldSwapChain);
	void CreateImageViews(VkDevice device);
	void CreateRenderPass(VulkanContext* pVkContext);
	void CreateFrameBuffers(VkDevice device);
	void CreateDepthResources(VulkanContext* pVkContext);

	//VkSwapchainKHR m_SwapChain;
	std::vector<VkImage> m_SwapChainImages; // created by VkSwapchainKHR, destroyed by VkSwapchainKHR
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;
	std::vector<unique_ptr_del<VkImageView_Ext>> m_SwapChainImageViews;
	std::vector<unique_ptr_del<VkFramebuffer>> m_SwapChainFramebuffers;
	unique_ptr_del<VkRenderPass> m_RenderPass;

	std::vector<unique_ptr_del<VkDepthImage_Ext>> m_DepthImage;
	std::vector<unique_ptr_del<VkImageView_Ext>> m_DepthImageView;
};