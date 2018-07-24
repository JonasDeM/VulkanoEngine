#pragma once
#include "stdafx.h"

#include <stdexcept>
#include "VkPhysicalDevice_Ext.h"
#include <set>

uint32_t VkPhysicalDevice_Ext::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(*this, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

VkFormat VkPhysicalDevice_Ext::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(*this, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

void VkPhysicalDevice_Ext::ControlAndRecordFeatures(const VkSurfaceKHR surface, std::vector<const char*> deviceExtensions)
{
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);

	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	// check if the device has all the necesary queue families
	GetQueueFamilyIndices(surface);
	if (!m_QueueFamilyIndices->isComplete())
		m_IsSuitable = false;

	//check if it supports all the extensions we need
	if (!CheckDeviceExtensionSupport(deviceExtensions))
		m_IsSuitable = false;

	//check if it has adequate swapchain support
	QuerySwapChainSupport(surface);
	if (m_SwapChainSupportDetails->formats.empty() || m_SwapChainSupportDetails->presentModes.empty())
		m_IsSuitable = false;

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(*this, &supportedFeatures);
	if (!supportedFeatures.samplerAnisotropy)
		m_IsSuitable = false;
}

bool VkPhysicalDevice_Ext::CheckDeviceExtensionSupport(std::vector<const char*> deviceExtensions) const
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(*this, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(*this, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
		if (requiredExtensions.empty())
			return true;
	}
	return requiredExtensions.empty();
}

void VkPhysicalDevice_Ext::GetQueueFamilyIndices(const VkSurfaceKHR surface) const
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(*this, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(*this, &queueFamilyCount, queueFamilies.data());

	for (uint32_t i = 0; i < queueFamilyCount; ++i)
	{
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			m_QueueFamilyIndices->graphicsFamily = i;
		}

		VkBool32 presentSupport;
		vkGetPhysicalDeviceSurfaceSupportKHR(*this, i, surface, &presentSupport);
		if (queueFamilies[i].queueCount > 0 && presentSupport)
		{
			m_QueueFamilyIndices->presentFamily = i;
		}
		// look for/add other queue families

		if (m_QueueFamilyIndices->isComplete()) {
			break;
		}
	}
}



void VkPhysicalDevice_Ext::QuerySwapChainSupport(VkSurfaceKHR surface) const
{
	//capabilities
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*this, surface, &m_SwapChainSupportDetails->capabilities);

	//formats
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(*this, surface, &formatCount, nullptr);
	if (formatCount > 0)
	{
		m_SwapChainSupportDetails->formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(*this, surface, &formatCount, m_SwapChainSupportDetails->formats.data());
	}

	uint32_t presentCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(*this, surface, &presentCount, nullptr);
	if (presentCount > 0)
	{
		m_SwapChainSupportDetails->presentModes.resize(presentCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(*this, surface, &presentCount, m_SwapChainSupportDetails->presentModes.data());
	}
}
