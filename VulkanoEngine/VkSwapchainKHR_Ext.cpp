#pragma once

#include "stdafx.h"

#include "VkSwapchainKHR_Ext.h"
#include "VkImageView_Ext.h"
#include "VulkanUtils.h"
#include "VkBasicSampler_Ext.h"
#include "VulkanContext.h"
#include "VkDevice_Ext.h"


void VkSwapchainKHR_Ext::CreateSwapChain(VulkanContext* pVkContext, const VkExtent2D& windowExtent, VkSwapchainKHR oldSwapChain)
{
	VkSurfaceKHR surface = *pVkContext->GetSurface();
	auto swapChainSupport = pVkContext->GetVkPhysicalDevice()->GetSwapChainSupportDetails(true, surface);
	auto queueFamilyIndices = pVkContext->GetVkPhysicalDevice()->GetQueueFamilyIndices();
	//image format
	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport->formats);
	//triple buffering or vsync or fifo
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport->presentModes);
	// resolution of images
	VkExtent2D extent = GetActualSwapExtent(swapChainSupport->capabilities, windowExtent);


	//length of the image queue
	auto imageCount = swapChainSupport->capabilities.minImageCount + 1;
	if (swapChainSupport->capabilities.maxImageCount > 0 && imageCount > swapChainSupport->capabilities.maxImageCount)
		imageCount = swapChainSupport->capabilities.maxImageCount;

	//queue families used
	uint32_t queueFamilyIndicesArray[] = { static_cast<uint32_t>(queueFamilyIndices->graphicsFamily)
		, static_cast<uint32_t>(queueFamilyIndices->presentFamily) };

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; //1 for 2D, 2 for 3D i guess
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (queueFamilyIndicesArray[0] != queueFamilyIndicesArray[1])
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	createInfo.preTransform = swapChainSupport->capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = oldSwapChain;

	//VkSwapchainKHR newSwapChain;
	VkDevice device = *pVkContext->GetVkDevice();
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, this->BasePtr()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	//store necesarry info in member variables
	m_SwapChainExtent = extent;
	m_SwapChainImageFormat = surfaceFormat.format;
	// image handles
	vkGetSwapchainImagesKHR(device, *this, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, *this, &imageCount, m_SwapChainImages.data());
}

void VkSwapchainKHR_Ext::CreateDepthResources(VulkanContext* pVkContext)
{
	VkDevice device = *pVkContext->GetVkDevice();

	auto format = VkDepthImage_Ext::GetDepthFormat(pVkContext->GetVkPhysicalDevice());
	int imageCount = (int)m_SwapChainImages.size();
	m_DepthImage.resize(imageCount);
	m_DepthImageView.resize(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		m_DepthImage[i] = CreateExtendedHandle(new VkDepthImage_Ext(pVkContext, GetExtent()), device);
		m_DepthImageView[i] = CreateExtendedHandle(new VkImageView_Ext(device, *m_DepthImage[i], format, VK_IMAGE_ASPECT_DEPTH_BIT), device);
		m_DepthImage[i]->TransitionImageLayout(pVkContext, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}

void VkSwapchainKHR_Ext::CreateImageViews(VkDevice device)
{
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); ++i)
	{
		m_SwapChainImageViews[i] = CreateExtendedHandle(new VkImageView_Ext(device, m_SwapChainImages[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT), device);
	}
}

void VkSwapchainKHR_Ext::CreateRenderPass(VulkanContext* pVkContext)
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = m_SwapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//won't be using stencils now
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0; //index in the attachment description array
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = VkDepthImage_Ext::GetDepthFormat(pVkContext->GetVkPhysicalDevice());
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	//dependencies
	//It assumes that the transition occurs at the start of the pipeline
	//, but we haven't acquired the image yet at that point!
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; //VK_SUBPASS_EXTERNAL refers to the implicit subpass before or after the render pass depending on whether it is specified in srcSubpass or dstSubpass
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	//Creation
	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	createInfo.pAttachments = attachments.data();
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpass;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	VkDevice device = *pVkContext->GetVkDevice();
	m_RenderPass = CreateHandle<VkRenderPass>(vkDestroyRenderPass, device);
	if (vkCreateRenderPass(device, &createInfo, nullptr, m_RenderPass.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void VkSwapchainKHR_Ext::CreateFrameBuffers(VkDevice device)
{
	m_SwapChainFramebuffers.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); ++i)
	{
		//put the image views as attachemnt to the buffers
		std::array<VkImageView, 2> attachments = {
			*m_SwapChainImageViews[i],
			*m_DepthImageView[i]
		};

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		//  You can only use a framebuffer with the render passes that it is compatible with (roughly means same nr and type of attachments)
		createInfo.renderPass = *m_RenderPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size()); //see (create) renderpass
		createInfo.pAttachments = attachments.data();
		createInfo.width = m_SwapChainExtent.width;
		createInfo.height = m_SwapChainExtent.height;
		createInfo.layers = 1; //Our swap chain images are single images

		
		m_SwapChainFramebuffers[i] = CreateHandle<VkFramebuffer>(vkDestroyFramebuffer, device);
		if (vkCreateFramebuffer(device, &createInfo, nullptr, m_SwapChainFramebuffers[i].get()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

