#pragma once
#include "stdafx.h"

#include "VkInstance_Ext.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <GLFW/glfw3.h>

VkInstance_Ext::VkInstance_Ext(bool enableValidationLayers, const std::vector<const char*>& validationLayers)
{
	if (enableValidationLayers && !CheckValidationLayerSupport(validationLayers)) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// optional but is good practice, can also improve performance for example if you write engine name = UE4, it COULD handle special behaviour of UE4
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Demo";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "VulkanoEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;


	auto reqExtensions = ChooseVulkanExtensions(enableValidationLayers);
	uint32_t availableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

	std::cout << "[VULKAN]: Listing available vulkan-extensions: " << std::endl;
	for (const auto& extension : availableExtensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}

	//checking if the required extensions are available
	for (const char* reqExtension : reqExtensions)
	{
		if (!std::any_of(availableExtensions.begin(), availableExtensions.end(),
			[reqExtension](VkExtensionProperties extension) {return strcmp(reqExtension, extension.extensionName) == 0; }))
		{
			std::stringstream error;
			error << "The required extension \"" << reqExtension << "\" is not available." << std::endl;
			throw std::runtime_error(error.str());
		}
	}

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = (uint32_t)reqExtensions.size();
	createInfo.ppEnabledExtensionNames = reqExtensions.data();
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = (uint32_t)validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	//check and throw runtime error
	vkCreateInstance(&createInfo, nullptr, this->BasePtr());
}

bool VkInstance_Ext::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//checking if the required extensions are available
	for (const char* layerName : validationLayers)
	{
		if (!std::any_of(availableLayers.begin(), availableLayers.end(),
			[layerName](VkLayerProperties layerProperties) {return strcmp(layerName, layerProperties.layerName) == 0; }))
		{
			return false;
		}
	}
	return true;
}

std::vector<const char*> VkInstance_Ext::ChooseVulkanExtensions(bool enableValidationLayers)
{
	//Here i define all extensions i want to use

	std::vector<const char*> extensions;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (uint32_t i = 0; i < glfwExtensionCount; i++) {
		extensions.push_back(glfwExtensions[i]);
	}

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}
