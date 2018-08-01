#pragma once
#include "DeriveableHandle.h"
#include "HandleUtilities.h"
#include <vector>
#include <memory>

class VulkanContext;
class VkImageView_Ext;
class VkDepthImage_Ext;
struct QueueFamilyIndices;

class VkSwapchainKHR_Ext : public DeriveableHandle<VkSwapchainKHR>
{
public:

	VkSwapchainKHR_Ext(VulkanContext* pVkContext, const VkExtent2D& windowExtent, VkSwapchainKHR oldSwapChain, int desiredFrameBuffers = 3);

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
	static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D GetActualSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities, const VkExtent2D& windowExtent) const;

	void CreateSwapChain(VulkanContext* pVkContext, const VkExtent2D& windowExtent, VkSwapchainKHR oldSwapChain, int desiredFrameBuffers);
	void CreateImageViews(VkDevice device);
	void CreateRenderPass(VulkanContext* pVkContext);
	void CreateFrameBuffers(VkDevice device);
	void CreateDepthResources(VulkanContext* pVkContext);

	std::vector<VkImage> m_SwapChainImages; // created by VkSwapchainKHR, destroyed by VkSwapchainKHR
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;
	std::vector<unique_ptr_del<VkImageView_Ext>> m_SwapChainImageViews;
	std::vector<unique_ptr_del<VkFramebuffer>> m_SwapChainFramebuffers;
	unique_ptr_del<VkRenderPass> m_RenderPass;

	std::vector<unique_ptr_del<VkDepthImage_Ext>> m_DepthImages;
	std::vector<unique_ptr_del<VkImageView_Ext>> m_DepthImageViews;
};