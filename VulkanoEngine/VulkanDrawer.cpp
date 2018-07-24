#include "stdafx.h"
#include "VulkanDrawer.h"
#include <vulkan/vulkan.h>
#include "VkSwapchainKHR_Ext.h"
#include "VkPhysicalDevice_Ext.h"
#include "VkDevice_Ext.h"
#include "VkInstance_Ext.h"
#include "Surface.h"
#include "Debug.h"
#include "SceneManager.h"
#include "VkPipelineManager.h"
#include "GameSettings.h"
#include "Debug.h"

VulkanDrawer::VulkanDrawer(GLFWwindow* targetWindow)
{
	m_pVkInstance = CreateExtendedHandle(new VkInstance_Ext(m_EnableValidationLayers));
	SetupDebugCallback();
	m_pVkSurface = CreateExtendedHandle(new VkSurface_Ext(*m_pVkInstance, targetWindow), *m_pVkInstance);
	PickPhysicalDevice();
	m_pVkDevice = CreateExtendedHandle(new VkDevice_Ext(*m_pVkPhysicalDevice, m_DeviceExtensions));
	auto indices = m_pVkPhysicalDevice->GetQueueFamilyIndices();
	vkGetDeviceQueue(*m_pVkDevice, indices->graphicsFamily, 0, &m_VkGraphicsQueue);
	vkGetDeviceQueue(*m_pVkDevice, indices->presentFamily, 0, &m_VkPresentQueue);
	CreateCommandPools();
	CreateDescriptorPool();
	int width, height;
	glfwGetWindowSize(targetWindow, &width, &height);
	m_pVkSwapchain = CreateExtendedHandle(new VkSwapchainKHR_Ext(this, VkExtent2D{ static_cast<uint32_t>(width),static_cast<uint32_t>(height) }, VK_NULL_HANDLE), *m_pVkDevice);
	CreateSemaphores();
}

void VulkanDrawer::ClearFlags(VkContextFlags flags)
{
	m_Flags = static_cast<VkContextFlags>((m_Flags) & (!flags));
}

bool VulkanDrawer::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//checking if the required extensions are available
	for (const char* layerName : m_ValidationLayers)
	{
		if (!std::any_of(availableLayers.begin(), availableLayers.end(),
			[layerName](VkLayerProperties layerProperties) {return strcmp(layerName, layerProperties.layerName) == 0; }))
		{
			return false;
		}
	}
	return true;
}

//Physical device
void VulkanDrawer::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(*m_pVkInstance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(*m_pVkInstance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		auto myPhysDevice = std::make_unique<VkPhysicalDevice_Ext>(device, *m_pVkSurface, m_DeviceExtensions);
		if (myPhysDevice->IsSuitable()) {
			m_pVkPhysicalDevice = std::move(myPhysDevice);
			break;
		}
	}

	if (m_pVkPhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

//extension choices
std::vector<const char*> VulkanDrawer::ChooseVulkanExtensions() const
{
	//Here i define all extensions i want to use

	std::vector<const char*> extensions;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (uint32_t i = 0; i < glfwExtensionCount; i++) {
		extensions.push_back(glfwExtensions[i]);
	}

	if (m_EnableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

//Validation Layers
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDrawer::DebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	Debug::Log(Debug::LogLevel::Vulkan, msg);
	return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugReportCallbackEXT * pCallback)
{
	auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks * pAllocator)
{
	auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

void VulkanDrawer::SetupDebugCallback()
{
	if (!m_EnableValidationLayers)
		return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = DebugCallback;

	m_pVkCallback = CreateHandle<VkDebugReportCallbackEXT>(DestroyDebugReportCallbackEXT, *m_pVkInstance);
	if (CreateDebugReportCallbackEXT(*m_pVkInstance, &createInfo, nullptr, m_pVkCallback.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug callback!");
	}
}

void VulkanDrawer::CreateCommandPools()
{
	//GraphicsCommandPool
	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.queueFamilyIndex = m_pVkPhysicalDevice->GetQueueFamilyIndices()->graphicsFamily; // cause these will be drawing commands
	createInfo.flags = 0;

	m_pVkGraphicsCommandPool = CreateHandle<VkCommandPool>(vkDestroyCommandPool, *m_pVkDevice);
	if (vkCreateCommandPool(*m_pVkDevice, &createInfo, nullptr, m_pVkGraphicsCommandPool.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics command pool!");
	}

	createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	m_pVkGraphicsCommandPoolTransient = CreateHandle<VkCommandPool>(vkDestroyCommandPool, *m_pVkDevice);
	if (vkCreateCommandPool(*m_pVkDevice, &createInfo, nullptr, m_pVkGraphicsCommandPoolTransient.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create transient graphics command pool!");
	}
}

void VulkanDrawer::CreateSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	m_pVkImageAvailableSemaphore = CreateHandle<VkSemaphore>(vkDestroySemaphore, *m_pVkDevice);
	if (vkCreateSemaphore(*m_pVkDevice, &semaphoreInfo, nullptr, m_pVkImageAvailableSemaphore.get()) != VK_SUCCESS) {

		throw std::runtime_error("failed to create image available semaphore!");
	}

	m_pVkRenderFinishedSemaphore = CreateHandle<VkSemaphore>(vkDestroySemaphore, *m_pVkDevice);
	if (vkCreateSemaphore(*m_pVkDevice, &semaphoreInfo, nullptr, m_pVkRenderFinishedSemaphore.get()) != VK_SUCCESS) {

		throw std::runtime_error("failed to create semaphores!");
	}
}

void VulkanDrawer::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;

	m_pVkDescriptorPool = CreateHandle<VkDescriptorPool>(vkDestroyDescriptorPool, *m_pVkDevice);
	if (vkCreateDescriptorPool(*m_pVkDevice, &poolInfo, nullptr, m_pVkDescriptorPool.get()) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

//todo
void VulkanDrawer::RecreateVkSwapChain(GameSettings* settings)
{
	vkDeviceWaitIdle(*m_pVkDevice);
	m_pVkSwapchain = CreateExtendedHandle(new VkSwapchainKHR_Ext(this, settings->GetWindowExtent(), *m_pVkSwapchain), *m_pVkDevice);

	VkPipelineManager::DestroyInstance();
	VkPipelineManager::GetInstance()->Initialize(this);
	CreateDrawCommandBuffers(settings);
}

void VulkanDrawer::VkDrawFrame(GameSettings* settings)
{
	//1.Acquire an image from the swap chain
	//2.Execute the command buffer with that image as attachment in the framebuffer
	//3.Return the image to the swap chain for presentation

	//these actions will be executed asynchronously, we want to do them in fixed order >> semaphores

	//1.
	uint32_t imageIndex;
	auto result = vkAcquireNextImageKHR(*m_pVkDevice, *m_pVkSwapchain, std::numeric_limits<uint64_t>::max() //disables timeout
		, *m_pVkImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateVkSwapChain(settings);
		return;
	}
	if (HasFlags(VkContextFlags::InvalidDrawCommandBuffers)) {
		//Debug::Log(Debug::LogLevel::Info, L"Rerecorded VkDrawCmdBuffers");
		CreateDrawCommandBuffers(settings);
		//return; 
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("failed to acquire swap chain image!");

	//2.
	std::vector<VkCommandBuffer> graphicsCommands{ m_DrawCommandBuffers[imageIndex] };
	//we define WHERE(waitstage) to wait for WHAT semaphore 
	VkSemaphore waitSemaphores[] = { *m_pVkImageAvailableSemaphore };
	//and which semaphore to signal next
	VkSemaphore signalSemaphores[] = { *m_pVkRenderFinishedSemaphore };
	//We want to wait with writing colors to the image until it's available
	//so we're specifying the stage of the graphics pipeline that writes to the color attachment. 
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	//Submitting the commands to the desired queue
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = (uint32_t)graphicsCommands.size();
	submitInfo.pCommandBuffers = graphicsCommands.data();
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_VkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw std::runtime_error("failed to submit draw command buffer!");

	//3.
	VkSwapchainKHR swapChains[] = { *m_pVkSwapchain };
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // to get the return values of multiple swapchainpresentation

	result = vkQueuePresentKHR(m_VkPresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		RecreateVkSwapChain(settings);
	else if (result != VK_SUCCESS)
		throw std::runtime_error("failed to present swap chain image!");

}

//per scene change
void VulkanDrawer::CreateDrawCommandBuffers(GameSettings* settings)
{
	vkDeviceWaitIdle(*m_pVkDevice); //wait for all async processes to complete
									//frees any previous buffers
	if (m_DrawCommandBuffers.size() > 0) {
		vkFreeCommandBuffers(*m_pVkDevice, *m_pVkGraphicsCommandPool, (uint32_t)m_DrawCommandBuffers.size(), m_DrawCommandBuffers.data());
	}

	//allocate buffers for the (in this case drawing) commands
	m_DrawCommandBuffers.resize(m_pVkSwapchain->GetAmountImages());
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = *m_pVkGraphicsCommandPool;
	allocInfo.commandBufferCount = (uint32_t)m_DrawCommandBuffers.size();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;


	if (vkAllocateCommandBuffers(*m_pVkDevice, &allocInfo, m_DrawCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	//start command buffer recording
	for (uint32_t i = 0; i < m_DrawCommandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;
		//The pInheritanceInfo parameter is only relevant for secondary command buffers. It specifies which state to inherit from the calling primary command buffers.

		//It's not possible to append commands to a buffer at a later time.

		//this begins recording the commands we want to store in the buffer
		vkBeginCommandBuffer(m_DrawCommandBuffers[i], &beginInfo);

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_pVkSwapchain->GetRenderPass();
		renderPassInfo.framebuffer = m_pVkSwapchain->GetFrameBuffer(i);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_pVkSwapchain->GetExtent();

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = settings->GetClearColor();
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		//begin renderpass
		vkCmdBeginRenderPass(m_DrawCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE); //embedded in the primary command buffer
																									//bind our graphicspipeline
		Debug::RecordVulkanDrawCommands(m_DrawCommandBuffers[i]);
		SceneManager::RecordVulkanDrawCommands(m_DrawCommandBuffers[i]);
		//end the render pass
		vkCmdEndRenderPass(m_DrawCommandBuffers[i]);
		//end the recording of our commands
		if (vkEndCommandBuffer(m_DrawCommandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	ClearFlags(VkContextFlags::InvalidDrawCommandBuffers);
}

